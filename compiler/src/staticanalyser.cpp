#include "staticanalyser.h"

bool IsTruthy(const TypeData &cond)
{
    if (cond.isArray)
        return false;

    return (cond.type == 1) || (cond.type == 2) || (cond.type == 3);
}

bool StaticAnalyser::MatchInitialiserToStruct(const std::vector<TypeData> &member, const std::vector<TypeData> &init)
{
    if (member.size() != init.size())
        return false;

    for (size_t i = 0; i < member.size(); i++)
    {
        if (!Symbols.CanAssign(member[i], init[i]))
            return false;
    }

    return true;
}

void StaticAnalyser::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + '\n' + err + '\n');
    throw e;
}

void StaticAnalyser::operator()(std::vector<std::shared_ptr<Stmt>> &_prog)
{
    prog = _prog;
    for (index = 0; index < prog.size(); index++)
        TypeCheck(prog[index]);
}

void StaticAnalyser::TypeCheck(std::shared_ptr<Stmt> &s)
{
    s->Type(*this);
}

//-----------------EXPRESSIONS---------------------//

TypeData StaticAnalyser::TypeOfLiteral(Literal *l)
{
    return l->t;
}

TypeData StaticAnalyser::TypeOfUnary(Unary *u)
{
    TypeData opType = u->right->Type(*this);
    TypeInfo info = {VOID_TYPE, u->op.type, opType};

    if (CheckUnaryOperatorUse(info))
    {
        u->t = OperatorMap.at(info);
        return u->t;
    }
    else
        TypeError(u->Loc(), "Cannot use operator: " + ToString(u->op.type) + " on operand of type: " + ToString(opType));

    return VOID_TYPE;
}

TypeData StaticAnalyser::TypeOfBinary(Binary *b)
{
    TypeData lType = b->left->Type(*this);
    TypeData rType = b->right->Type(*this);
    TypeInfo info = {lType, b->op.type, rType};

    if (CheckBinaryOperatorUse(info))
    {
        b->t = GetBinaryOperatorType(info);
        return b->t;
    }
    else
        TypeError(b->Loc(), "Cannot use operator: " + ToString(b->op.type) + " on operands of type: " + ToString(lType) + " and: " + ToString(rType));
    return VOID_TYPE;
}

TypeData StaticAnalyser::TypeOfAssign(Assign *a)
{
    VarReference *targetAsVr = dynamic_cast<VarReference *>(a->target.get());
    TypeData valType = a->val->Type(*this);

    if (targetAsVr != nullptr)
    {
        VarID *vidTarg = Symbols.GetVar(targetAsVr->name);
        if (vidTarg == nullptr)
            TypeError(targetAsVr->Loc(), "Variable '" + targetAsVr->name + "' has not been defined");

        TypeData varType = vidTarg->type;

        if (!Symbols.CanAssign(varType, valType))
            TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(varType));

        targetAsVr->isArray = varType.isArray;
        targetAsVr->t = varType;
        a->val->t = varType;
        a->t = varType;

        return varType;
    }

    TypeData targetType = a->target->Type(*this);
    if (!Symbols.CanAssign(targetType, valType))
        TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(targetType));
    a->val->t = targetType;
    return targetType;
}

TypeData StaticAnalyser::TypeOfVarReference(VarReference *vr)
{
    VarID *vid = Symbols.GetVar(vr->name);
    if (vid == nullptr)
        TypeError(vr->Loc(), "Variable '" + vr->name + "' has not been defined");

    vr->t = vid->type;
    return vid->type;
}

TypeData StaticAnalyser::TypeOfFunctionCall(FunctionCall *fc)
{
    std::vector<TypeData> argtypes;

    for (auto &e : fc->args)
        argtypes.push_back(e->Type(*this));

    FuncID *fid = Symbols.GetFunc(fc->name, fc->templates, argtypes);
    std::cout << fid->name << " num_args " << fid->argtypes.size() << " parse_index = " << fid->parseIndex << std::endl;
    assert(false);
    if (fid == nullptr)
    {
        std::string errStr = fc->name + "(";
        for (size_t i = 0; i < fc->args.size(); i++)
        {
            errStr += ToString(argtypes[i]);
            if (i != fc->args.size() - 1)
                errStr += ", ";
        }
        errStr += ");";
        TypeError(fc->Loc(), "Function '" + errStr + "' has not been defined");
    }

    fc->t = fid->ret;
    return fid->ret;
}

TypeData StaticAnalyser::TypeOfArrayIndex(ArrayIndex *ai)
{
    TypeData nameT = ai->name->Type(*this);

    if (!nameT.isArray && nameT != STRING_TYPE)
        TypeError(ai->Loc(), "Cannot index into object of type " + ToString(nameT));

    TypeData idxT = ai->index->Type(*this);

    if (idxT != INT_TYPE)
        TypeError(ai->Loc(), "Index into string/array must be of type int not " + ToString(idxT));

    if (nameT.isArray)
    {
        ai->t = nameT;
        ai->t.isArray--;
    }

    if (nameT == STRING_TYPE)
        ai->t = CHAR_TYPE;

    return ai->t;
}

TypeData StaticAnalyser::TypeOfBracedInitialiser(BracedInitialiser *bi)
{
    if (bi->size == 0)
        return VOID_TYPE;

    std::vector<TypeData> types;

    for (auto &init : bi->init)
        types.push_back(init->Type(*this));

    if (bi->t == VOID_TYPE)
    {
        TypeData first = types[0];
        for (size_t i = 1; i < types.size(); i++)
        {
            if (!Symbols.CanAssign(first, types[i]))
                TypeError(bi->init[i]->Loc(), "Types of expression in a braced initialiser must be assignable to each other");
        }
        bi->t = first;
        bi->t.isArray++;
        return bi->t;
    }
    if (bi->t.isArray)
    {
        TypeData toCompare = bi->t;
        toCompare.isArray--;
        for (size_t i = 0; i < types.size(); i++)
        {
            if (!Symbols.CanAssign(toCompare, types[i]))
                TypeError(bi->init[i]->Loc(), "Type of expression in a braced initaliser must be assignable to the type specified at the beginning");
        }
        return bi->t;
    }
    else
    {
        if (bi->t.type < NUM_DEF_TYPES)
            TypeError(bi->Loc(), "Cannot declare a braced initialiser with " + ToString(bi->t) + " type");

        StructID *strct = Symbols.GetStruct(bi->t);
        if (strct == nullptr)
            TypeError(bi->Loc(), "Expect valid struct name before a braced initialiser");

        if (MatchInitialiserToStruct(strct->memTypes, types))
        {
            for (size_t i = 0; i < bi->init.size(); i++)
                bi->init[i]->t = strct->memTypes[i];

            return strct->type;
        }
        else
            TypeError(bi->Loc(), "Types in braced initialiser do not match the types required by the type specified at its beginning " + ToString(bi->t));
    }
    // dummy return, never reached
    return VOID_TYPE;
}

TypeData StaticAnalyser::TypeOfDynamicAllocArray(DynamicAllocArray *da)
{
    TypeData sizeType = da->size->Type(*this);
    TypeData intType = INT_TYPE;

    if (sizeType != intType)
        TypeError(da->Loc(), "Size of dynamically allocated array must have type int");

    return da->t;
}

TypeData StaticAnalyser::TypeOfFieldAccess(FieldAccess *fa)
{
    TypeData accessorType = fa->accessor->Type(*this);

    StructID *sid = Symbols.GetStruct(accessorType);
    if (sid == nullptr)
        TypeError(fa->Loc(), "Type " + ToString(accessorType) + " cannot be accesed into");

    Symbols.depth++;

    for (const auto &kv : sid->nameTypes)
        Symbols.AddVar(kv.second, kv.first);

    TypeData accesseeType = fa->accessee->Type(*this);
    fa->t = accesseeType;

    Symbols.CleanUpCurDepth();
    Symbols.depth--;
    return accesseeType;
}

TypeData StaticAnalyser::TypeOfTypeCast(TypeCast *c)
{
    TypeData newT = c->type;
    TypeData oldT = c->arg->Type(*this);

    bool isDownCast = Symbols.CanAssign(newT, oldT);
    bool isUpCast = Symbols.CanAssign(oldT, newT);

    if (!isDownCast && !isUpCast)
        TypeError(c->Loc(), "Invalid cast");

    c->isDownCast = isDownCast;

    c->t = newT;
    return c->t;
}

//------------------STATEMENTS---------------------//

void StaticAnalyser::TypeOfExprStmt(ExprStmt *es)
{
    es->exp->Type(*this);
}

void StaticAnalyser::TypeOfDeclaredVar(DeclaredVar *dv)
{
    if (Symbols.IsVarInScope(dv->name))
        TypeError(dv->Loc(), "Variable " + dv->name + " is already defined");

    if (dv->value != nullptr)
    {
        TypeData valType = dv->value->Type(*this);

        if (!Symbols.CanAssign(dv->t, valType))
            TypeError(dv->Loc(), "Cannot assign a value of type " + ToString(valType) + " to variable of type " + ToString(dv->t));

        if (dv->t.type < NUM_DEF_TYPES)
            dv->value->t = dv->t;
    }
    Symbols.AddVar(dv->t, dv->name);
}

void StaticAnalyser::TypeOfBlock(Block *b)
{
    Symbols.depth++;
    size_t preBlockSize = Symbols.vars.size();

    for (auto &stmt : b->stmts)
        stmt->Type(*this);

    Symbols.PopUntilSized(preBlockSize);
    Symbols.depth--;
}

void StaticAnalyser::TypeOfIfStmt(IfStmt *i)
{
    if (!IsTruthy(i->cond->Type(*this)))
        TypeError(i->Loc(), "Condition of and if statement must be 'turthy'");

    i->thenBranch->Type(*this);

    if (i->elseBranch != nullptr)
        i->elseBranch->Type(*this);
}

void StaticAnalyser::TypeOfWhileStmt(WhileStmt *ws)
{
    if (!IsTruthy(ws->cond->Type(*this)))
        TypeError(ws->Loc(), "Condition of a while statment must be 'truthy'");

    ws->body->Type(*this);
}

void StaticAnalyser::TypeOfFuncDecl(FuncDecl *fd)
{
    if (Symbols.funcs.size() > MAX_OPCODE)
        TypeError(fd->Loc(), "Maximum number of functions is " + std::to_string(MAX_OPCODE));

    curFunc = fd;

    std::vector<TypeData> templates;
    for (auto &t : fd->templates)
        templates.push_back(t.first);

    Symbols.depth++;
    FunctionType kind = templates.size() == 0 ? FunctionType::USER_DEFINED : FunctionType::USER_DEFINED_TEMPLATE;

    Symbols.AddFunc(FuncID(fd->ret, fd->name, templates, fd->argtypes, kind, index));

    if (fd->templates.size() > 0)
        return;

    size_t preFuncSize = Symbols.vars.size();

    if (fd->argtypes.size() != fd->paramIdentifiers.size())
    {
        std::cerr << "SOMETHING FUNDAMENTAL WENT WRONG HERE" << std::endl;
        exit(14);
    }

    for (size_t j = 0; j < fd->argtypes.size(); j++)
        Symbols.AddVar(fd->argtypes[j], fd->paramIdentifiers[j]);

    for (auto &s : fd->body)
        s->Type(*this);

    for (auto &e : fd->preConds)
        e->Type(*this);

    Symbols.PopUntilSized(preFuncSize);
    Symbols.depth--;
    curFunc = nullptr;
}

void StaticAnalyser::TypeOfReturn(Return *r)
{
    if (Symbols.depth == 0)
        TypeError(r->Loc(), "Cannot return from outside of a function");

    if (r->retVal == nullptr)
    {
        if (curFunc->ret != TypeData(false, 0))
            TypeError(r->Loc(), "Non-void function cannot return void");

        if (r->postConds.size() != 0)
            TypeError(r->Loc(), "Void function cannot have post conditions");
    }

    assert(curFunc != nullptr);
    TypeData ret = r->retVal->Type(*this);

    if (ret != curFunc->ret)
        TypeError(r->Loc(), "Return type does not match declaration");

    // typechecking post conditions against function arguments
    StaticAnalyser tempSA;

    tempSA.Symbols.AddVar(curFunc->ret, "result");

    for (size_t i = 0; i < curFunc->argtypes.size(); i++)
        tempSA.Symbols.AddVar(curFunc->argtypes[i], curFunc->paramIdentifiers[i]);

    for (auto e : r->postConds)
    {
        if (e->Type(tempSA) != BOOL_TYPE)
            TypeError(r->Loc(), "Post condition expressions must be bools");
    }
}

void StaticAnalyser::TypeOfStructDecl(StructDecl *sd)
{
    if (sd->decls.size() > MAX_OPCODE)
        TypeError(sd->Loc(), "Structs can only have a maximum of " + std::to_string(MAX_OPCODE) + " members");

    Symbols.depth++;

    StructID s;
    s.type = GetTypeNameMap()[sd->name];

    if (sd->parent != VOID_TYPE)
    {
        StructID *parent = Symbols.GetStruct(sd->parent);

        if (parent == nullptr)
            TypeError(sd->Loc(), "Invalid parent struct");

        s.parent = parent->type;

        for (const std::string &name : parent->memberNames)
            s.memberNames.push_back(name);

        for (const TypeData &type : parent->memTypes)
            s.memTypes.push_back(type);

        for (const auto &e : parent->init)
            s.init.push_back(e);

        for (const auto kv : parent->nameTypes)
            s.nameTypes[kv.first] = kv.second;
    }

    s.isNull = false;

    for (auto &d : sd->decls)
    {
        d->Type(*this);
        DeclaredVar *asDV = dynamic_cast<DeclaredVar *>(d.get());

        if (asDV == nullptr)
            TypeError(d->Loc(), "The body of struct declarations can only consist of variable declarations");

        s.memberNames.push_back(asDV->name);
        s.memTypes.push_back(asDV->t);

        if (asDV->value != nullptr)
            s.init.push_back(asDV->value);

        s.nameTypes[asDV->name] = asDV->t;
    }

    if (sd->parent.isArray)
        TypeError(sd->Loc(), "Parent of struct cannot be an array");

    Symbols.strcts.push_back(s);
    Symbols.CleanUpCurDepth();
    Symbols.depth--;
}

// for now all functions of library are imported
void StaticAnalyser::TypeOfImportStmt(ImportStmt *is)
{
    std::vector<std::string> libraryFuncs;
    assert(is->libraries.size() > 0);
    for (const auto library : is->libraries)
    {
        libraryFuncs = Symbols.GetLibraryFunctionNames(library);
        for (auto &lf : libraryFuncs)
        {
            FuncID func = Symbols.ParseLibraryFunction(lf);
            Symbols.AddCLibFunc(func);

            if (Symbols.funcs.size() > MAX_OPCODE)
                TypeError(is->Loc(), "Cannot import more than " + std::to_string(MAX_OPCODE) + " library functions in total");
        }
    }
}

void StaticAnalyser::TypeOfThrow(Throw *t)
{
    t->exp->Type(*this);
}

void StaticAnalyser::TypeOfTryCatch(TryCatch *tc)
{
    tc->tryClause->Type(*this);
    Symbols.AddVar(tc->catchVar.first, tc->catchVar.second);
    tc->catchClause->Type(*this);
}

//-----------------EXPRESSIONS---------------------//

TypeData Literal::Type(StaticAnalyser &t)
{
    return t.TypeOfLiteral(this);
}

TypeData Unary::Type(StaticAnalyser &t)
{
    return t.TypeOfUnary(this);
}

TypeData Binary::Type(StaticAnalyser &t)
{
    return t.TypeOfBinary(this);
}

TypeData Assign::Type(StaticAnalyser &t)
{
    return t.TypeOfAssign(this);
}

TypeData VarReference::Type(StaticAnalyser &t)
{
    return t.TypeOfVarReference(this);
}

TypeData FunctionCall::Type(StaticAnalyser &t)
{
    return t.TypeOfFunctionCall(this);
}

TypeData ArrayIndex::Type(StaticAnalyser &t)
{
    return t.TypeOfArrayIndex(this);
}

TypeData BracedInitialiser::Type(StaticAnalyser &t)
{
    return t.TypeOfBracedInitialiser(this);
}

TypeData DynamicAllocArray::Type(StaticAnalyser &t)
{
    return t.TypeOfDynamicAllocArray(this);
}

TypeData FieldAccess::Type(StaticAnalyser &t)
{
    return t.TypeOfFieldAccess(this);
}

TypeData TypeCast::Type(StaticAnalyser &t)
{
    return t.TypeOfTypeCast(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Type(StaticAnalyser &t)
{
    t.TypeOfExprStmt(this);
}

void DeclaredVar::Type(StaticAnalyser &t)
{
    t.TypeOfDeclaredVar(this);
}

void Block::Type(StaticAnalyser &t)
{
    t.TypeOfBlock(this);
}

void IfStmt::Type(StaticAnalyser &t)
{
    t.TypeOfIfStmt(this);
}

void WhileStmt::Type(StaticAnalyser &t)
{
    t.TypeOfWhileStmt(this);
}

void FuncDecl::Type(StaticAnalyser &t)
{
    t.TypeOfFuncDecl(this);
}

void Return::Type(StaticAnalyser &t)
{
    t.TypeOfReturn(this);
}

void StructDecl::Type(StaticAnalyser &t)
{
    t.TypeOfStructDecl(this);
}

void ImportStmt::Type(StaticAnalyser &t)
{
    t.TypeOfImportStmt(this);
}

void Break::Type(StaticAnalyser &)
{
}

void Throw::Type(StaticAnalyser &t)
{
    t.TypeOfThrow(this);
}

void TryCatch::Type(StaticAnalyser &t)
{
    t.TypeOfTryCatch(this);
}