#include "typechecker.h"

bool TypeChecker::CanAssign(const TypeData &varType, const TypeData &valType)
{
    if (varType.isArray != valType.isArray)
        return false;

    if (varType.type == 6 || valType.type == 6)
        return true;
    else if (varType.type == 1 && valType.type == 2)
        return true;
    else if (varType.type == 2 && valType.type == 1)
        return true;

    if (varType.type > 6 && valType.type > 6)
    {
        if (varType == valType)
            return true;

        size_t valLoc = Symbols.FindStruct(valType);
        TypeData parent = Symbols.strcts[valLoc].parent;

        TypeData voidType = {false, 0};

        if (parent == voidType)
            return varType == valType;

        parent.isArray = valType.isArray;
        return CanAssign(varType, parent);
    }

    return varType == valType;
}

bool IsTruthy(const TypeData &cond)
{
    if (cond.isArray)
        return false;

    return (cond.type == 1) || (cond.type == 2) || (cond.type == 3);
}

bool TypeChecker::MatchInitialiserToStruct(const std::vector<TypeData> &member, const std::vector<TypeData> &init)
{
    if (member.size() != init.size())
        return false;

    for (size_t i = 0; i < member.size(); i++)
    {
        if (!CanAssign(member[i], init[i]))
            return false;
    }

    return true;
}

void TypeChecker::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + '\n' + err + '\n');
    throw e;
}

void TypeChecker::TypeCheck(std::shared_ptr<Stmt> &s)
{
    s->Type(*this);
}

//-----------------EXPRESSIONS---------------------//

TypeData TypeChecker::TypeOfLiteral(Literal *l)
{
    return l->t;
}

TypeData TypeChecker::TypeOfUnary(Unary *u)
{
    TypeData opType = u->right->Type(*this);
    TypeInfo info = {{false, 0}, u->op.type, opType};

    if (CheckUnaryOperatorUse(info))
    {
        u->t = OperatorMap.at(info);
        return u->t;
    }
    else
        TypeError(u->Loc(), "Cannot use operator: " + ToString(u->op.type) + " on operand of type: " + ToString(opType));

    return {false, 0};
}

TypeData TypeChecker::TypeOfBinary(Binary *b)
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
    return {false, 0};
}

TypeData TypeChecker::TypeOfAssign(Assign *a)
{
    VarReference *targetAsVr = dynamic_cast<VarReference *>(a->target.get());

    TypeData valType = a->val->Type(*this);

    if (targetAsVr != nullptr)
    {
        size_t varIndex = Symbols.FindVarByName(targetAsVr->name);

        if (varIndex == SIZE_MAX)
            TypeError(targetAsVr->Loc(), "Variable reference " + targetAsVr->name + " has not been defined before");

        TypeData varType = Symbols.vars[varIndex].type;

        if (!CanAssign(varType, valType))
            TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(varType));

        targetAsVr->isArray = varType.isArray;
        targetAsVr->t = varType;
        a->val->t = varType;
        a->t = varType;

        return varType;
    }

    TypeData targetType = a->target->Type(*this);
    if (!CanAssign(targetType, valType))
        TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(targetType));

    return targetType;
}

TypeData TypeChecker::TypeOfVarReference(VarReference *vr)
{
    size_t index = Symbols.FindVarByName(vr->name);

    if (index == SIZE_MAX)
        TypeError(vr->Loc(), "Variable reference " + vr->name + " has not been defined before");

    vr->t = Symbols.vars[index].type;
    return Symbols.vars[index].type;
}

TypeData TypeChecker::TypeOfFunctionCall(FunctionCall *fc)
{
    std::vector<TypeData> argtypes;

    for (auto &e : fc->args)
        argtypes.push_back(e->Type(*this));

    size_t index = Symbols.FindFunc(fc->name, argtypes);

    if (index == SIZE_MAX)
    {
        size_t nativeIndex = Symbols.FindNativeFunctions(argtypes);
        if (nativeIndex != SIZE_MAX)
        {
            fc->t = Symbols.nativeFunctions[nativeIndex].ret;
            return Symbols.nativeFunctions[nativeIndex].ret;
        }
        std::string err = "Function '" + fc->name + "(";

        for(const auto &arg : argtypes)
            err += ToString(arg) + ", ";

        err += ")' has not been defined before";
        TypeError(fc->Loc(), err);
    }

    if (index > UINT8_MAX)
        TypeError(fc->Loc(), "Cannot have more than " + std::to_string(UINT8_MAX) + " functions");

    fc->t = Symbols.funcs[index].ret;
    return Symbols.funcs[index].ret;
}

TypeData TypeChecker::TypeOfArrayIndex(ArrayIndex *ai)
{
    TypeData nameT = ai->name->Type(*this);
    TypeData stringT = {false, 4};

    if (!nameT.isArray && nameT != stringT)
        TypeError(ai->Loc(), "Cannot index into object of type " + ToString(nameT));

    TypeData idxT = ai->index->Type(*this);
    TypeData intT = {false, 1};

    if (idxT != intT)
        TypeError(ai->Loc(), "Index into string/array must be of type int not " + ToString(idxT));

    if (nameT.isArray)
    {
        ai->t = nameT;
        ai->t.isArray--;
    }

    if (nameT == stringT)
        ai->t = {false, 5};

    return ai->t;
}

TypeData TypeChecker::TypeOfBracedInitialiser(BracedInitialiser *bi)
{
    if (bi->size == 0)
        return {false, 0};

    std::vector<TypeData> types;

    for (auto &init : bi->init)
        types.push_back(init->Type(*this));

    TypeData voidT = {false, 0};
    if (bi->t == voidT)
    {
        TypeData first = types[0];
        for (size_t i = 1; i < types.size(); i++)
        {
            if (!CanAssign(first, types[i]))
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
            if (!CanAssign(toCompare, types[i]))
                TypeError(bi->init[i]->Loc(), "Type of expression in a braced initaliser must be assignable to the type specified at the beginning");
        }
        return bi->t;
    }
    else
    {
        if (bi->t.type < NUM_DEF_TYPES)
            TypeError(bi->Loc(), "Cannot declare a braced initialiser with " + ToString(bi->t) + " type");

        size_t strctNum = Symbols.FindStruct(bi->t);
        if (strctNum == SIZE_MAX)
            TypeError(bi->Loc(), "Expect valid struct name in front of braced initialiser");

        if (MatchInitialiserToStruct(Symbols.strcts[strctNum].memTypes, types))
        {
            for (size_t i = 0; i < bi->init.size(); i++)
                bi->init[i]->t = Symbols.strcts[strctNum].memTypes[i];

            return Symbols.strcts[strctNum].type;
        }
        else
            TypeError(bi->Loc(), "Types in braced initialiser do not match the types required by the type specified at its beginning " + ToString(bi->t));
    }
    // dummy return, never reached
    return {false, 0};
}

TypeData TypeChecker::TypeOfDynamicAllocArray(DynamicAllocArray *da)
{
    TypeData sizeType = da->size->Type(*this);
    TypeData intType = {false, 1};

    if (sizeType != intType)
        TypeError(da->Loc(), "Size of dynamically allocated array must have type int");

    return da->t;
}

TypeData TypeChecker::TypeOfFieldAccess(FieldAccess *fa)
{
    TypeData accessorType = fa->accessor->Type(*this);

    size_t structIndex = Symbols.FindStruct(accessorType);

    if (structIndex == SIZE_MAX)
        TypeError(fa->Loc(), "Type " + ToString(accessorType) + " cannot be accesed into");

    StructID s = Symbols.strcts[structIndex];
    Symbols.depth++;
    size_t preVarSize = Symbols.vars.size();

    for (const auto &kv : s.nameTypes)
        Symbols.AddVar(kv.second, kv.first);

    TypeData accesseeType = fa->accessee->Type(*this);
    fa->t = accesseeType;

    Symbols.PopUntilSized(preVarSize);
    Symbols.depth--;
    return accesseeType;
}

TypeData TypeChecker::TypeOfTypeCast(TypeCast *c)
{
    TypeData newT = c->type;
    TypeData oldT = c->arg->Type(*this);

    bool isDownCast = CanAssign(newT, oldT);
    bool isUpCast = CanAssign(oldT, newT);

    if (!isDownCast && !isUpCast)
        TypeError(c->Loc(), "Invalid cast");

    c->isDownCast = isDownCast;

    c->t = newT;
    return c->t;
}

//------------------STATEMENTS---------------------//

TypeData TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    return es->exp->Type(*this);
}

TypeData TypeChecker::TypeOfDeclaredVar(DeclaredVar *dv)
{
    if (Symbols.IsVarInScope(dv->name))
        TypeError(dv->Loc(), "Variable " + dv->name + " is already defined");

    Symbols.AddVar(dv->t, dv->name);

    if (dv->value != nullptr)
    {
        TypeData valType = dv->value->Type(*this);

        if (!CanAssign(dv->t, valType))
            TypeError(dv->Loc(), "Cannot assign a value of type " + ToString(valType) + " to variable of type " + ToString(dv->t));

        if (dv->t.type < NUM_DEF_TYPES)
            dv->value->t = dv->t;
    }

    return {false, 0};
}

TypeData TypeChecker::TypeOfBlock(Block *b)
{
    Symbols.depth++;
    size_t preBlockSize = Symbols.vars.size();

    for (auto &stmt : b->stmts)
        stmt->Type(*this);

    Symbols.PopUntilSized(preBlockSize);
    Symbols.depth--;
    return {false, 0};
}

TypeData TypeChecker::TypeOfIfStmt(IfStmt *i)
{
    if (!IsTruthy(i->cond->Type(*this)))
        TypeError(i->Loc(), "Condition of and if statement must be 'turthy'");

    i->thenBranch->Type(*this);

    if (i->elseBranch != nullptr)
        i->elseBranch->Type(*this);

    return {false, 0};
}

TypeData TypeChecker::TypeOfWhileStmt(WhileStmt *ws)
{
    if (!IsTruthy(ws->cond->Type(*this)))
        TypeError(ws->Loc(), "Condition of a while statment must be 'truthy'");

    ws->body->Type(*this);
    return {false, 0};
}

TypeData TypeChecker::TypeOfFuncDecl(FuncDecl *fd)
{
    if (Symbols.funcs.size() > UINT8_MAX)
        TypeError(fd->Loc(), "Maximum number of functions is " + std::to_string(UINT8_MAX));

    Symbols.depth++;
    Symbols.AddFunc(fd->ret, fd->name, fd->argtypes);
    size_t preFuncSize = Symbols.vars.size();

    if (fd->argtypes.size() != fd->paramIdentifiers.size())
    {
        std::cout << "SOMETHING WENT WRONG HERE" << std::endl;
        exit(14);
    }

    for (size_t j = 0; j < fd->argtypes.size(); j++)
    {
        Symbols.AddVar(fd->argtypes[j], fd->paramIdentifiers[j]);
    }

    for (auto &s : fd->body)
    {
        s->Type(*this);
    }

    Symbols.PopUntilSized(preFuncSize);
    Symbols.depth--;
    return {false, 0};
}

TypeData TypeChecker::TypeOfReturn(Return *r)
{
    if (Symbols.depth == 0)
        TypeError(r->Loc(), "Cannot return from outside of a function");
    if (r->retVal == nullptr)
        return {false, 0};
    return r->retVal->Type(*this);
}

TypeData TypeChecker::TypeOfStructDecl(StructDecl *sd)
{
    if (sd->decls.size() > UINT8_MAX)
        TypeError(sd->Loc(), "Structs can only have a maximum of " + std::to_string(UINT8_MAX) + " members");

    Symbols.depth++;

    StructID s;
    s.type = GetTypeNameMap()[sd->name];

    TypeData voidT = {false, 0};

    if (sd->parent != voidT)
    {
        size_t parIndex = Symbols.FindStruct(sd->parent);

        if (parIndex == SIZE_MAX)
            TypeError(sd->Loc(), "Invalid parent struct");

        StructID par = Symbols.strcts[parIndex];
        s.parent = par.type;

        for (const std::string &name : par.memberNames)
            s.memberNames.push_back(name);

        for (const TypeData &type : par.memTypes)
            s.memTypes.push_back(type);

        for (const auto &e : par.init)
            s.init.push_back(e);

        for (const auto kv : par.nameTypes)
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

    Symbols.strcts.push_back(s);
    Symbols.depth--;

    return {false, 0};
}

//-----------------EXPRESSIONS---------------------//

TypeData Literal::Type(TypeChecker &t)
{
    return t.TypeOfLiteral(this);
}

TypeData Unary::Type(TypeChecker &t)
{
    return t.TypeOfUnary(this);
}

TypeData Binary::Type(TypeChecker &t)
{
    return t.TypeOfBinary(this);
}

TypeData Assign::Type(TypeChecker &t)
{
    return t.TypeOfAssign(this);
}

TypeData VarReference::Type(TypeChecker &t)
{
    return t.TypeOfVarReference(this);
}

TypeData FunctionCall::Type(TypeChecker &t)
{
    return t.TypeOfFunctionCall(this);
}

TypeData ArrayIndex::Type(TypeChecker &t)
{
    return t.TypeOfArrayIndex(this);
}

TypeData BracedInitialiser::Type(TypeChecker &t)
{
    return t.TypeOfBracedInitialiser(this);
}

TypeData DynamicAllocArray::Type(TypeChecker &t)
{
    return t.TypeOfDynamicAllocArray(this);
}

TypeData FieldAccess::Type(TypeChecker &t)
{
    return t.TypeOfFieldAccess(this);
}

TypeData TypeCast::Type(TypeChecker &t)
{
    return t.TypeOfTypeCast(this);
}

//------------------STATEMENTS---------------------//

TypeData ExprStmt::Type(TypeChecker &t)
{
    return t.TypeOfExprStmt(this);
}

TypeData DeclaredVar::Type(TypeChecker &t)
{
    return t.TypeOfDeclaredVar(this);
}

TypeData Block::Type(TypeChecker &t)
{
    return t.TypeOfBlock(this);
}

TypeData IfStmt::Type(TypeChecker &t)
{
    return t.TypeOfIfStmt(this);
}

TypeData WhileStmt::Type(TypeChecker &t)
{
    return t.TypeOfWhileStmt(this);
}

TypeData FuncDecl::Type(TypeChecker &t)
{
    return t.TypeOfFuncDecl(this);
}

TypeData Return::Type(TypeChecker &t)
{
    return t.TypeOfReturn(this);
}

TypeData StructDecl::Type(TypeChecker &t)
{
    return t.TypeOfStructDecl(this);
}