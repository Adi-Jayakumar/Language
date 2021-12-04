#include "staticanalyser.h"

inline bool IsTruthy(const TypeData &td)
{
    return td == INT_TYPE || td == BOOL_TYPE;
}

void StaticAnalyser::Analyse(std::vector<SP<Stmt>> &_program)
{
    program = _program;
    for (parse_index = 0; parse_index < program.size(); parse_index++)
        program[parse_index]->Analyse(*this);
}

void StaticAnalyser::StaticAnalysisError(Token loc, std::string err)
{
    Error e = Error("[STATIC ANALYSIS ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void StaticAnalyser::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void StaticAnalyser::SymbolError(Token loc, std::string err)
{
    Error e = Error("[SYMBOL ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

TypeData StaticAnalyser::AnalyseLiteral(Literal *l)
{
    return l->t;
}

TypeData StaticAnalyser::AnalyseUnary(Unary *u)
{
    TypeData right = u->right->Analyse(*this);

    if (!symbols.CheckOperatorUse(VOID_TYPE, u->op.type, right))
        TypeError(u->op, "Cannot use oprand of type " + symbols.ToString(right) + " with operator " + u->op.literal);

    u->t = symbols.OperatorResult(VOID_TYPE, u->op.type, right);
    return u->t;
}

TypeData StaticAnalyser::AnalyseBinary(Binary *b)
{
    TypeData left = b->left->Analyse(*this);
    TypeData right = b->right->Analyse(*this);

    if (!symbols.CheckOperatorUse(left, b->op.type, right))
        TypeError(b->op, "Cannot use operands of type " + symbols.ToString(left) + " and " + symbols.ToString(right) + "with operator " + b->op.literal);

    b->t = symbols.OperatorResult(left, b->op.type, right);
    return b->t;
}

TypeData StaticAnalyser::AnalyseAssign(Assign *a)
{
    /*
    Valid assignment targets
    1) VarReference
    2) ArrayIndex
    3) FieldAccess
    */
    TypeData value = a->val->Analyse(*this);
    TypeData target = a->target->Analyse(*this);

    if (!symbols.CanAssign(target, value))
        StaticAnalysisError(a->target->Loc(), "Cannot assign a value of type " + symbols.ToString(value) + " to target of type " + symbols.ToString(target));

    if (dynamic_cast<VarReference *>(a->target.get()) == nullptr &&
        dynamic_cast<ArrayIndex *>(a->target.get()) == nullptr &&
        dynamic_cast<FieldAccess *>(a->target.get()) == nullptr)
        StaticAnalysisError(a->target->Loc(), "Invalid assignment target");

    a->t = target;
    return a->t;
}

TypeData StaticAnalyser::AnalyseVarReference(VarReference *vr)
{
    VarID *vid = symbols.GetVar(vr->name);
    if (vid == nullptr)
        SymbolError(vr->Loc(), "Variable '" + vr->name + "' has not been defined");
    vr->t = vid->type;
    return vr->t;
}

TypeData StaticAnalyser::AnalyseFunctionCall(FunctionCall *fc)
{
    std::vector<TypeData> args;
    for (auto &e : fc->args)
        args.push_back(e->Analyse(*this));

    FuncID *fid = symbols.GetFunc(fc->name, fc->templates, args);
    if (fid == nullptr)
    {
        std::ostringstream out;
        out << fc->name;

        if (fc->templates.size() > 0)
        {
            out << "<|";
            for (size_t i = 0; i < fc->templates.size(); i++)
            {
                symbols.PrintType(out, fc->templates[i]);
                if (i != fc->templates.size() - 1)
                    out << ", ";
            }
            out << "|>";
        }

        out << "(";
        if (args.size() > 0)
        {
            for (size_t i = 0; i < args.size(); i++)
            {
                symbols.PrintType(out, args[i]);
                if (i != args.size() - 1)
                    out << ", ";
            }
        }
        out << ")";
        SymbolError(fc->Loc(), "Function '" + out.str() + "' has not been defined yet");
    }

    fc->t = fid->ret;
    return fc->t;
}

TypeData StaticAnalyser::AnalyseArrayIndex(ArrayIndex *ai)
{
    TypeData name = ai->name->Analyse(*this);

    if (!name.isArray && name != STRING_TYPE)
        TypeError(ai->Loc(), "Cannot index into object of type " + symbols.ToString(name));

    TypeData index = ai->index->Analyse(*this);
    if (index != INT_TYPE)
        TypeError(ai->index->Loc(), "Indices can only be of type int not " + symbols.ToString(index));

    if (name.isArray)
    {
        name.isArray--;
        ai->t = name;
    }
    else
        ai->t = CHAR_TYPE;
    return ai->t;
}

TypeData StaticAnalyser::AnalyseBracedInitialiser(BracedInitialiser *bi)
{
    if (bi->size > MAX_OPRAND)
        StaticAnalysisError(bi->Loc(), "Braced initialisers can only have " + std::to_string(MAX_OPRAND) + " elements");

    TypeData biType = bi->t;

    std::vector<TypeData> args;
    for (auto &e : bi->init)
        args.push_back(e->Analyse(*this));

    if (biType.isArray)
    {
        TypeData target = biType;
        target.isArray--;

        for (size_t i = 0; i < bi->init.size(); i++)
        {
            if (!symbols.CanAssign(target, args[i]))
                TypeError(bi->init[i]->Loc(), "Cannot assign object of type " + symbols.ToString(args[i]) + " to the required type of the array specified at the beginning of the braced initialiser");
            bi->init[i]->t = target;
        }

        bi->t = biType;
    }
    else
    {
        StructID *sid = symbols.GetStruct(biType);
        if (sid == nullptr)
            TypeError(bi->Loc(), "Invalid struct name in braced initialiser");
        if (sid->nameTypes.size() != args.size())
            TypeError(bi->Loc(), "Number of arguments in braced initialiser is not equal to the number of arguments in struct declaration");

        for (size_t i = 0; i < bi->size; i++)
        {
            if (!symbols.CanAssign(sid->nameTypes[i].second, args[i]))
                TypeError(bi->init[i]->Loc(), "Object in struct braced initialiser of type " + symbols.ToString(args[i]) + " cannot be assigned to object of type " + symbols.ToString(sid->nameTypes[i].second));
            bi->init[i]->t = sid->nameTypes[i].second;
        }

        bi->t = sid->type;
    }
    return bi->t;
}

TypeData StaticAnalyser::AnalyseDynamicAllocArray(DynamicAllocArray *da)
{
    if (!da->t.isArray)
        TypeError(da->Loc(), "Must have array type");
    TypeData size = da->size->Analyse(*this);
    if (size != INT_TYPE)
        TypeError(da->Loc(), "Size of dynamically allocated array must have type int not " + symbols.ToString(size));

    return da->t;
}

TypeData StaticAnalyser::AnalyseFieldAccess(FieldAccess *fa)
{
    TypeData accessor = fa->accessor->Analyse(*this);

    StructID *sid = symbols.GetStruct(accessor);
    if (sid == nullptr)
        TypeError(fa->Loc(), "Type " + symbols.ToString(accessor) + " cannot be accessed into");

    VarReference *vAccessee = dynamic_cast<VarReference *>(fa->accessee.get());
    if (vAccessee == nullptr)
        TypeError(fa->accessee->Loc(), "Only an identifier can be used as a field accessee");

    // index of accessee in the underlying array
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < sid->nameTypes.size(); i++)
    {
        if (sid->nameTypes[i].first == vAccessee->name)
        {
            index = i;
            break;
        }
    }

    if (index == SIZE_MAX)
        SymbolError(fa->accessee->Loc(), "Struct " + sid->name + " does not have member " + vAccessee->name);

    fa->t = sid->nameTypes[index].second;
    return fa->t;
}

TypeData StaticAnalyser::AnalyseTypeCast(TypeCast *tc)
{
    TypeData old = tc->arg->Analyse(*this);
    TypeData nw = tc->t;

    bool isDownCast = symbols.CanAssign(nw, old);
    bool isUpCast = symbols.CanAssign(old, nw);

    if (!isDownCast && !isUpCast)
        TypeError(tc->Loc(), "Cannot cast " + symbols.ToString(old) + " to " + symbols.ToString(nw));

    return tc->t;
}

TypeData StaticAnalyser::AnalyseSequence(Sequence *s)
{
    // TODO - Make general term only depend on function arguments
    TypeData start = s->start->Analyse(*this);
    if (start != INT_TYPE)
        TypeError(s->start->Loc(), "Start of sequence must be of type int");

    TypeData step = s->step->Analyse(*this);
    if (step != INT_TYPE)
        TypeError(s->start->Loc(), "Step of sequence must be of type int");

    TypeData end = s->end->Analyse(*this);
    if (end != INT_TYPE)
        TypeError(s->start->Loc(), "End of sequence must be of type int");

    VarID *vid = symbols.GetVar(s->var->name);
    if (vid != nullptr)
        StaticAnalysisError(s->var->Loc(), "Indexing variable is already defined");

    symbols.depth++;
    symbols.AddVar(INT_TYPE, s->var->name, 0);

    TypeData term = s->term->Analyse(*this);
    if (term != INT_TYPE)
        TypeError(s->term->Loc(), "Term of Sequence must of of type int");

    symbols.CleanUpCurDepth();
    symbols.depth--;

    if (!symbols.CheckOperatorUse(INT_TYPE, s->op, INT_TYPE))
        StaticAnalysisError(s->term->Loc(), "The operator of a sequence must be able to take 2 integers as arguments");

    if (symbols.OperatorResult(INT_TYPE, s->op, INT_TYPE) != INT_TYPE)
        StaticAnalysisError(s->term->Loc(), "The result of using the operator on 2 integers must be an integer");

    s->t = INT_TYPE;
    return s->t;
}

//------------------STATEMENTS---------------------//

void StaticAnalyser::AnalyseExprStmt(ExprStmt *es)
{
    es->exp->Analyse(*this);
}

void StaticAnalyser::AnalyseDeclaredVar(DeclaredVar *dv)
{
    if (symbols.vars.size() > MAX_OPRAND)
        StaticAnalysisError(dv->Loc(), "Maximum number of variables is " + std::to_string(MAX_OPRAND));

    VarID *vid = symbols.GetVar(dv->name);
    if (vid != nullptr && vid->depth == symbols.depth)
        SymbolError(dv->Loc(), "Variable '" + symbols.ToString(dv->t) + " " + dv->name + "' has already been defined in this scope");

    if (dv->value != nullptr)
    {
        TypeData val = dv->value->Analyse(*this);
        if (!symbols.CanAssign(dv->t, val))
            TypeError(dv->Loc(), "Cannot initialise variable '" + dv->name + "' of type " + symbols.ToString(dv->t) + " with expression of type " + symbols.ToString(val));
    }
    else
    {
        if (symbols.depth == 0)
            SymbolError(dv->Loc(), "Global variable must be initialised");
    }
    symbols.AddVar(dv->t, dv->name, 0);
}

void StaticAnalyser::AnalyseBlock(Block *b)
{
    symbols.depth++;
    for (auto &stmt : b->stmts)
        stmt->Analyse(*this);
    symbols.depth--;
}

void StaticAnalyser::AnalyseIfStmt(IfStmt *i)
{
    TypeData cond = i->cond->Analyse(*this);
    if (!IsTruthy(cond))
        TypeError(i->cond->Loc(), "Condition of if statement must be convertible to bool");

    i->then_branch->Analyse(*this);
    if (i->else_branch != nullptr)
        i->else_branch->Analyse(*this);
}

void StaticAnalyser::AnalyseWhileStmt(WhileStmt *ws)
{
    TypeData cond = ws->cond->Analyse(*this);
    if (!IsTruthy(cond))
        TypeError(ws->cond->Loc(), "Condition of a loop must be convertible to bool");

    ws->body->Analyse(*this);
}

void StaticAnalyser::AnalyseFuncDecl(FuncDecl *fd)
{
    std::vector<TypeData> argtypes;
    for (auto &arg : fd->params)
        argtypes.push_back(arg.first);

    std::vector<TypeData> templates;
    for (auto &t : fd->templates)
        templates.push_back(t.first);

    FuncID *isThere = symbols.GetFunc(fd->name, templates, argtypes);
    if (isThere != nullptr)
    {
        std::ostringstream out;

        if (fd->templates.size() > 0)
        {
            out << "template<|";
            for (size_t i = 0; i < fd->templates.size(); i++)
            {
                out << fd->templates[i].second;
                if (i != fd->templates.size() - 1)
                    out << ", ";
            }
            out << "|>";
        }

        symbols.PrintType(out, fd->ret);
        out << " " << fd->name << "(";

        if (fd->params.size() > 0)
        {
            for (size_t i = 0; i < fd->params.size(); i++)
            {
                symbols.PrintType(out, fd->params[i].first);
                out << " " << fd->params[i].second;
                if (i != fd->params.size() - 1)
                    out << ", ";
            }
        }
        out << ")";
        SymbolError(fd->Loc(), "Function '" + out.str() + "' has already been defined");
    }

    symbols.AddFunc(FuncID(fd->ret, fd->name, templates, argtypes, FunctionType::USER_DEFINED, parse_index));

    if (fd->templates.size() > 0)
        return;

    symbols.depth++;
    for (auto &arg : fd->params)
        symbols.AddVar(arg.first, arg.second, 0);

    for (auto &stmt : fd->body)
        stmt->Analyse(*this);

    symbols.CleanUpCurDepth();
    symbols.depth--;
}

void StaticAnalyser::AnalyseReturn(Return *r)
{
    if (r->ret_val != nullptr)
        r->ret_val->Analyse(*this);
}

void StaticAnalyser::AnalyseStructDecl(StructDecl *sd)
{
    TypeData type = symbols.ResolveType(sd->name).value();

    if (type.type < NUM_DEF_TYPES)
        SymbolError(sd->Loc(), "Invalid struct name");

    TypeData parent = sd->parent;
    std::vector<SP<Expr>> init;
    std::vector<std::pair<std::string, TypeData>> nameTypes;

    if (parent != VOID_TYPE)
    {
        if (parent.isArray)
            TypeError(sd->Loc(), "Parent of a struct cannot be array");
        StructID *sidParent = symbols.GetStruct(parent);

        if (sidParent == nullptr)
            TypeError(sd->Loc(), "Invalid parent struct name");

        nameTypes.insert(nameTypes.end(), sidParent->nameTypes.begin(), sidParent->nameTypes.end());
    }

    for (auto &d : sd->decls)
    {
        DeclaredVar *asDV = dynamic_cast<DeclaredVar *>(d.get());
        if (asDV == nullptr)
            TypeError(d->Loc(), "The body of struct declarations can only consist of variable declarations");

        if (asDV->value != nullptr)
            StaticAnalysisError(asDV->value->Loc(), "Variable declarations inside struct declarations cannot have values");

        nameTypes.push_back({asDV->name, asDV->t});
    }

    symbols.AddStruct(StructID(sd->name, type, parent, nameTypes));
}

void StaticAnalyser::AnalyseImportStmt(ImportStmt *is)
{
    std::vector<std::string> libraryFuncs;
    for (const auto &library : is->libraries)
    {
        libraryFuncs = symbols.GetLibraryFunctionNames(library);
        for (auto &lf : libraryFuncs)
        {
            FuncID func = symbols.ParseLibraryFunction(lf, FunctionType::LIBRARY);
            symbols.AddCLibFunc(func);

            if (symbols.NumCFuncs() > MAX_OPRAND)
                StaticAnalysisError(is->Loc(), "Cannot import more than " + std::to_string(MAX_OPRAND) + " library functions in total");
        }
    }
}

void StaticAnalyser::AnalyseBreak(Break *)
{
}

void StaticAnalyser::AnalyseThrow(Throw *t)
{
    t->exp->Analyse(*this);
}

void StaticAnalyser::AnalyserTryCatch(TryCatch *tc)
{
    tc->try_clause->Analyse(*this);
    tc->catch_clause->Analyse(*this);
}

//-----------------EXPRESSIONS---------------------//

TypeData Literal::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseLiteral(this);
}

TypeData Unary::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseUnary(this);
}

TypeData Binary::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseBinary(this);
}

TypeData Assign::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseAssign(this);
}

TypeData VarReference::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseVarReference(this);
}

TypeData FunctionCall::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseFunctionCall(this);
}

TypeData ArrayIndex::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseArrayIndex(this);
}

TypeData BracedInitialiser::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseBracedInitialiser(this);
}

TypeData DynamicAllocArray::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseDynamicAllocArray(this);
}

TypeData FieldAccess::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseFieldAccess(this);
}

TypeData TypeCast::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseTypeCast(this);
}

TypeData Sequence::Analyse(StaticAnalyser &sa)
{
    return sa.AnalyseSequence(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseExprStmt(this);
}

void DeclaredVar::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseDeclaredVar(this);
}

void Block::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseBlock(this);
}

void IfStmt::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseIfStmt(this);
}

void WhileStmt::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseWhileStmt(this);
}

void FuncDecl::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseFuncDecl(this);
}

void Return::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseReturn(this);
}

void StructDecl::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseStructDecl(this);
}

void ImportStmt::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseImportStmt(this);
}

void Break::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseBreak(this);
}

void Throw::Analyse(StaticAnalyser &sa)
{
    sa.AnalyseThrow(this);
}

void TryCatch::Analyse(StaticAnalyser &sa)
{
    sa.AnalyserTryCatch(this);
}