#include "typechecker.h"

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

//-----------------EXPRESSIONS---------------------//

uint8_t TypeChecker::TypeOfLiteral(Literal *l)
{
    if(l == nullptr)
        return 0;
    return l->typeID;
}

uint8_t TypeChecker::TypeOfUnary(Unary *u)
{
    if (u == nullptr || u->right == nullptr)
        return 0;
    uint8_t opType = u->right->Type();
    TypeInfo info = {opType, u->op.type, 0};

    if (OperatorMap.find(info) != OperatorMap.end())
        return static_cast<size_t>(OperatorMap.at(info));

    return 0;
}

uint8_t TypeChecker::TypeOfBinary(Binary *b)
{
    if (b == nullptr || b->left == nullptr || b->right == nullptr)
        return 0;
    uint8_t lType = b->left->Type();
    uint8_t rType = b->right->Type();
    TypeInfo info = {lType, b->op.type, rType};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);

    return 0;
}

uint8_t TypeChecker::TypeOfAssign(Assign *a)
{
    return 0;
}

uint8_t TypeChecker::TypeOfVarReference(VarReference *vr)
{
    return 0;
}

//------------------STATEMENTS---------------------//

uint8_t TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    if (es == nullptr || es->exp == nullptr)
        return 0;
    return es->exp->Type();
}

uint8_t TypeChecker::TypeOfVar(DeclaredVar *v)
{
    if (v == nullptr || v->value == nullptr)
        return 0;
    uint8_t vType = v->tId;
    uint8_t vValType = v->value->Type();
    if(vType == vValType)
        return vType;
    return 0;
}

//-----------------EXPRESSIONS---------------------//

uint8_t Literal::Type()
{
    return TypeChecker::TypeOfLiteral(this);
}

uint8_t Unary::Type()
{
    return TypeChecker::TypeOfUnary(this);
}

uint8_t Binary::Type()
{
    return TypeChecker::TypeOfBinary(this);
}

uint8_t Assign::Type()
{
    return TypeChecker::TypeOfAssign(this);
}

uint8_t VarReference::Type()
{
    return TypeChecker::TypeOfVarReference(this);
}

//------------------STATEMENTS---------------------//

uint8_t ExprStmt::Type()
{
    return TypeChecker::TypeOfExprStmt(this);
}

uint8_t DeclaredVar::Type()
{
    return TypeChecker::TypeOfVar(this);
}