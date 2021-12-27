#include "staticanalyser.h"

inline bool IsTruthy(const TypeData &td)
{
    return td == INT_TYPE || td == BOOL_TYPE;
}

static void PrintFunctionCall(std::string &name,
                              std::vector<TypeData> &templates,
                              std::vector<TypeData> &args,
                              SymbolTable &symbols,
                              std::ostream &out)
{
    out << name;

    if (templates.size() > 0)
    {
        out << "<";
        for (size_t i = 0; i < templates.size(); i++)
        {
            symbols.PrintType(out, templates[i]);
            if (i != templates.size() - 1)
                out << ", ";
        }
        out << ">";
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
}

void StaticAnalyser::Analyse(std::vector<SP<Stmt>> &_program)
{
    program = _program;
    for (parse_index = 0; parse_index < program.size(); parse_index++)
        program[parse_index]->Analyse(*this);
}

void StaticAnalyser::ErrorPreamble(std::ostream &out, const std::string err_type)
{

    if (cur_func != nullptr)
    {
        out << "\n\n\n[" << err_type << "] In function '";

        TypeSubstituter *type_subst = template_stack.size() > 1 ? &template_stack[1] : cur_type_subst;

        symbols.PrintType(out, (*type_subst)[cur_func->ret]);
        out << " " << cur_func->name << "(";

        if (cur_func->params.size() > 0)
        {
            for (size_t i = 0; i < cur_func->params.size(); i++)
            {
                symbols.PrintType(out, (*type_subst)[cur_func->params[i].first]);
                out << " " << cur_func->params[i].second;
                if (i != cur_func->params.size() - 1)
                    out << ", ";
            }
        }

        out << ")'";

        if (template_stack.size() > 1)
        {
            for (size_t i = template_stack.size() - 1; i > 0; --i)
            {
                TypeSubstituter ts = template_stack[i];
                out << "\nInstantiated from the template declaration on line " << ts.loc.line << std::endl;
                for (auto &kv : ts.subst)
                {
                    if (kv.first.type >= NUM_DEF_TYPES)
                    {
                        symbols.PrintType(out, kv.first);
                        out << " = ";
                        symbols.PrintType(out, kv.second);
                        out << '\n';
                    }
                }
            }
            out << '\n';
        }
        out << "\nOn line ";
    }
    else
        out << "\n[" << err_type << "] on line ";
}

void StaticAnalyser::StaticAnalysisError(Token loc, std::string err)
{
    std::ostringstream out;
    ErrorPreamble(out, "STATIC ANALYSIS ERROR");

    out << std::to_string(loc.line) << " near '" << loc.literal << "'\n"
        << err << "\n";

    throw Error(out.str());
}

void StaticAnalyser::TypeError(Token loc, std::string err)
{
    std::ostringstream out;
    ErrorPreamble(out, "TYPE ERROR");

    out << std::to_string(loc.line) << " near '" << loc.literal << "'\n"
        << err << "\n";

    throw Error(out.str());
}

void StaticAnalyser::SymbolError(Token loc, std::string err)
{
    std::ostringstream out;
    ErrorPreamble(out, "SYMBOL ERROR");

    out << std::to_string(loc.line) << " near '" << loc.literal << "'\n"
        << err << "\n";

    throw Error(out.str());
}

TypeData StaticAnalyser::AnalyseLiteral(Literal *l)
{
    return (*cur_type_subst)[l->t];
}

TypeData StaticAnalyser::AnalyseUnary(Unary *u)
{
    TypeData right = u->right->Analyse(*this);
    std::optional<TypeData> result = symbols.OperatorResult(VOID_TYPE, u->op.type, right);

    if (!result)
        TypeError(u->op, "Cannot use oprand of type " + symbols.ToString(right) + " with operator " + u->op.literal);

    return u->t = (*cur_type_subst)[result.value()];
}

TypeData StaticAnalyser::AnalyseBinary(Binary *b)
{
    TypeData left = b->left->Analyse(*this);
    TypeData right = b->right->Analyse(*this);
    std::optional<TypeData> result = symbols.OperatorResult(left, b->op.type, right);

    if (!result)
        TypeError(b->op, "Cannot use operands of type " + symbols.ToString(left) + " and " + symbols.ToString(right) + " with operator " + b->op.literal);

    return b->t = (*cur_type_subst)[result.value()];
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

    return a->t = (*cur_type_subst)[target];
}

TypeData StaticAnalyser::AnalyseVarReference(VarReference *vr)
{
    std::optional<VarID> vid = symbols.GetVar(vr->name);
    if (!vid)
        SymbolError(vr->Loc(), "Variable '" + vr->name + "' has not been defined");

    return vr->t = (*cur_type_subst)[vid.value().type];
}

TypeData StaticAnalyser::AnalyseFunctionCall(FunctionCall *fc)
{
    std::vector<TypeData> args;
    for (auto &e : fc->args)
        args.push_back(e->Analyse(*this));

    std::optional<FuncID> fid = symbols.GetFunc(fc->name, args);
    if (!fid)
    {
        std::ostringstream out;
        PrintFunctionCall(fc->name, fc->templates, args, symbols, out);
        SymbolError(fc->Loc(), "Function '" + out.str() + "' has not been defined yet");
    }

    bool typestack_needs_pop = false;

    // TODO make sure that arguement types follow the pattern of the template argument types
    // with the substitution applied and throw error otherwise
    // TODO Make sure that the underlying functions are not typechecked everytime they are called
    // and only when we encounter an instantiation with a different set of template arguments
    if (fc->templates.size() > 0)
    {
        FuncDecl *decl = dynamic_cast<FuncDecl *>(program[fid->parse_index].get());
        assert(decl != nullptr);
        std::vector<TypeData> templates;

        for (auto &tmp : decl->templates)
            templates.push_back(tmp.first);

        std::vector<TypeData> new_types;
        for (auto &new_t : fc->templates)
            new_types.push_back((*cur_type_subst)[new_t]);

        RegisterTypeSubst(TypeSubstituter(templates, new_types, fc->Loc()));
        typestack_needs_pop = true;

        assert(decl->params.size() == fc->args.size());
        bool template_arg_error = false;
        for (size_t i = 0; i < decl->params.size(); i++)
        {
            if ((*cur_type_subst)[decl->params[i].first] != args[i])
            {
                template_arg_error = true;
                break;
            }
        }

        if (template_arg_error)
        {
            std::ostringstream out;
            PrintFunctionCall(fc->name, fc->templates, args, symbols, out);
            TypeError(fc->Loc(), "Arguments not equal to types specified by templates in function call '" + out.str() + "'");
        }

        std::optional<FuncID> is_already_init = symbols.IsInitialisedTemplateFunc(fc->name, fc->templates, args);

        if (is_already_init)
        {
            PopTypeSubst();
            return is_already_init->ret;
        }
        else
        {
            symbols.AddInitialisedTemplateFunc(FuncID((*cur_type_subst)[decl->ret], decl->name, new_types, args, FunctionType::USER_DEFINED, 0));
            analyse_template_func = true;
            decl->Analyse(*this);
            analyse_template_func = false;
        }
    }
    else
    {
        if (fid->templates.size() > 0)
            StaticAnalysisError(fc->Loc(), "Calling template function with no templates");
    }
    TypeData res = fc->t = (*cur_type_subst)[fid->ret];
    if (typestack_needs_pop)
        PopTypeSubst();
    return res;
}

TypeData StaticAnalyser::AnalyseArrayIndex(ArrayIndex *ai)
{
    TypeData name = ai->name->Analyse(*this);

    if (!name.is_array && name != STRING_TYPE)
        TypeError(ai->Loc(), "Cannot index into object of type " + symbols.ToString(name));

    TypeData index = ai->index->Analyse(*this);
    if (index != INT_TYPE)
        TypeError(ai->index->Loc(), "Indices can only be of type int not " + symbols.ToString(index));

    if (name.is_array)
    {
        name.is_array--;
        ai->t = name;
    }
    else
        ai->t = CHAR_TYPE;

    return (*cur_type_subst)[ai->t];
}

TypeData StaticAnalyser::AnalyseBracedInitialiser(BracedInitialiser *bi)
{
    if (bi->size > MAX_OPRAND)
        StaticAnalysisError(bi->Loc(), "Braced initialisers can only have " + std::to_string(MAX_OPRAND) + " elements");

    TypeData bi_type = bi->t;

    std::vector<TypeData> args;
    for (auto &e : bi->init)
        args.push_back(e->Analyse(*this));

    if (bi_type.is_array)
    {
        TypeData target = bi_type;
        target.is_array--;

        for (size_t i = 0; i < bi->init.size(); i++)
        {
            if (!symbols.CanAssign(target, args[i]))
                TypeError(bi->init[i]->Loc(), "Cannot assign object of type " + symbols.ToString(args[i]) + " to the required type of the array specified at the beginning of the braced initialiser");
            bi->init[i]->t = target;
        }

        bi->t = bi_type;
    }
    else
    {
        std::optional<StructID> sid = symbols.GetStruct(bi_type);
        if (sid)
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
    if (!da->t.is_array)
        TypeError(da->Loc(), "Must have array type");
    TypeData size = da->size->Analyse(*this);
    if (size != INT_TYPE)
        TypeError(da->Loc(), "Size of dynamically allocated array must have type int not " + symbols.ToString(size));

    return (*cur_type_subst)[da->t];
}

TypeData StaticAnalyser::AnalyseFieldAccess(FieldAccess *fa)
{
    TypeData accessor = fa->accessor->Analyse(*this);

    std::optional<StructID> sid = symbols.GetStruct(accessor);
    if (sid)
        TypeError(fa->Loc(), "Type " + symbols.ToString(accessor) + " cannot be accessed into");

    VarReference *vr_accessee = dynamic_cast<VarReference *>(fa->accessee.get());
    if (vr_accessee == nullptr)
        TypeError(fa->accessee->Loc(), "Only an identifier can be used as a field accessee");

    // index of accessee in the underlying array
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < sid->nameTypes.size(); i++)
    {
        if (sid->nameTypes[i].first == vr_accessee->name)
        {
            index = i;
            break;
        }
    }

    if (index == SIZE_MAX)
        SymbolError(fa->accessee->Loc(), "Struct " + sid->name + " does not have member " + vr_accessee->name);

    return fa->t = (*cur_type_subst)[sid->nameTypes[index].second];
}

TypeData StaticAnalyser::AnalyseTypeCast(TypeCast *tc)
{
    TypeData old = tc->arg->Analyse(*this);
    TypeData nw = tc->t;

    bool is_down_cast = symbols.CanAssign(nw, old);
    bool is_up_cast = symbols.CanAssign(old, nw);

    if (!is_down_cast && !is_up_cast)
        TypeError(tc->Loc(), "Cannot cast " + symbols.ToString(old) + " to " + symbols.ToString(nw));

    return (*cur_type_subst)[tc->t];
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

    std::optional<VarID> vid = symbols.GetVar(s->var->name);
    if (vid)
        StaticAnalysisError(s->var->Loc(), "Indexing variable is already defined");

    symbols.depth++;
    symbols.AddVar(INT_TYPE, s->var->name, 0);

    TypeData term = s->term->Analyse(*this);
    if (term != INT_TYPE)
        TypeError(s->term->Loc(), "Term of Sequence must of of type int");

    symbols.CleanUpCurDepth();
    symbols.depth--;

    if (!symbols.OperatorResult(INT_TYPE, s->op, INT_TYPE))
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

    std::optional<VarID> vid = symbols.GetVar(dv->name);
    if (vid && vid->depth == symbols.depth)
        SymbolError(dv->Loc(), "Variable '" + symbols.ToString(dv->t) + " " + dv->name + "' has already been defined in this scope");

    if (dv->value != nullptr)
    {
        TypeData val = dv->value->Analyse(*this);
        if (!symbols.CanAssign((*cur_type_subst)[dv->t], val))
            TypeError(dv->Loc(), "Cannot initialise variable '" + dv->name + "' of type " + symbols.ToString(dv->t) + " with expression of type " + symbols.ToString(val));
    }
    else
    {
        if (symbols.depth == 0)
            SymbolError(dv->Loc(), "Global variable must be initialised");
    }
    symbols.AddVar((*cur_type_subst)[dv->t], dv->name, 0);
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
    cur_func = fd;

    std::vector<TypeData> argtypes;
    for (auto &arg : fd->params)
        argtypes.push_back(arg.first);

    std::vector<TypeData> templates;
    for (const auto &temp : fd->templates)
    {
        symbols.AddTypeString(temp.first, temp.second);
        templates.push_back(temp.first);
    }

    // only check whether a function is already defined when we are not
    // analysing a template function instantiaion as that is sure to be
    // defined already
    if (fd->templates.size() == 0)
    {
        std::optional<FuncID> is_there = symbols.GetFunc(fd->name, argtypes);
        if (is_there)
        {
            std::ostringstream out;

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
    }

    if (fd->templates.size() == 0)
        symbols.AddFunc(FuncID(fd->ret, fd->name, templates, argtypes, FunctionType::USER_DEFINED, parse_index));
    else
        symbols.AddTemplateFunc(FuncID(fd->ret, fd->name, templates, argtypes, FunctionType::USER_DEFINED_TEMPLATE, parse_index));

    if (fd->templates.size() > 0 && !analyse_template_func)
        return;

    symbols.depth++;
    for (auto &arg : fd->params)
        symbols.AddVar(arg.first, arg.second, 0);

    for (auto &pre : fd->pre_conds)
    {
        if (pre->Analyse(*this) != BOOL_TYPE)
            TypeError(pre->Loc(), "Precondition must have type bool");
    }

    for (auto &stmt : fd->body)
        stmt->Analyse(*this);

    symbols.AddVar(fd->ret, "result", 0);
    if (fd->post_cond != nullptr)
    {
        if (fd->post_cond->Analyse(*this) != BOOL_TYPE)
            TypeError(fd->post_cond->Loc(), "Postcondition must have type bool");
    }

    symbols.CleanUpCurDepth();
    symbols.depth--;

    cur_func = nullptr;
}

void StaticAnalyser::AnalyseReturn(Return *r)
{
    // TODO Check this matches current function's return type
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
    std::vector<std::pair<std::string, TypeData>> name_types;

    if (parent != VOID_TYPE)
    {
        if (parent.is_array)
            TypeError(sd->Loc(), "Parent of a struct cannot be array");
        std::optional<StructID> sid_parent = symbols.GetStruct(parent);

        if (sid_parent)
            TypeError(sd->Loc(), "Invalid parent struct name");

        name_types.insert(name_types.end(), sid_parent->nameTypes.begin(), sid_parent->nameTypes.end());
    }

    for (auto &d : sd->decls)
    {
        DeclaredVar *as_dv = dynamic_cast<DeclaredVar *>(d.get());
        if (as_dv == nullptr)
            TypeError(d->Loc(), "The body of struct declarations can only consist of variable declarations");

        if (as_dv->value != nullptr)
            StaticAnalysisError(as_dv->value->Loc(), "Variable declarations inside struct declarations cannot have values");

        name_types.push_back({as_dv->name, as_dv->t});
    }

    symbols.AddStruct(StructID(sd->name, type, parent, name_types));
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