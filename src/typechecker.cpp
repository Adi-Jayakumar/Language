#include "typechecker.h"

bool CanAssign(const TypeData &varType, const TypeData &valType)
{
    if (varType.isArray != valType.isArray)
        return false;

    if (varType.type == 6 || valType.type == 6)
        return true;
    else if (varType.type == 1 && valType.type == 2)
        return true;
    else if (varType.type == 2 && valType.type == 1)
        return true;

    // if (varType.type > 6 && valType.type > 6)
    // {
    //     if (varType == valType)
    //         return true;

    //     size_t valLoc = ResolveStruct(valType);
    //     TypeData parent = structTypes[valLoc].parent;

    //     TypeData voidType = {false, 0};

    //     if (parent == voidType)
    //         return varType == valType;

    //     parent.isArray = valType.isArray;
    //     return CanAssign(varType, parent);
    // }

    return varType == valType;
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

TypeData TypeChecker::TypeOfFunctionCall(FunctionCall *)
{
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

TypeData TypeChecker::TypeOfBracedInitialiser(BracedInitialiser *)
{
}

TypeData TypeChecker::TypeOfDynamicAllocArray(DynamicAllocArray *da)
{
    TypeData sizeType = da->size->Type(*this);
    TypeData intType = {false, 1};

    if (sizeType != intType)
        TypeError(da->Loc(), "Size of dynamically allocated array must have type int");

    return da->t;
}

TypeData TypeChecker::TypeOfFieldAccess(FieldAccess *)
{
}

TypeData TypeChecker::TypeOfTypeCast(TypeCast *)
{
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

    for (auto &stmt : b->stmts)
        stmt->Type(*this);

    Symbols.depth--;
    return {false, 0};
}

TypeData TypeChecker::TypeOfIfStmt(IfStmt *)
{
}

TypeData TypeChecker::TypeOfWhileStmt(WhileStmt *)
{
}

TypeData TypeChecker::TypeOfFuncDecl(FuncDecl *)
{
}

TypeData TypeChecker::TypeOfReturn(Return *)
{
}

TypeData TypeChecker::TypeOfStructDecl(StructDecl *)
{
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