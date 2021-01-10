#include "typechecker.h"

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

uint8_t TypeChecker::TypeOfLiteral(Literal *l)
{
    return l->typeID;
}

uint8_t TypeChecker::TypeOfUnary(Unary *u)
{
    uint8_t opType = u->right->Type();
    TypeInfo info = {opType, u->op.type, 0};

    if(Operators.find(info) != Operators.end())
        return static_cast<size_t>(OperatorMap.at(info));

    return 0;
}

uint8_t TypeChecker::TypeOfBinary(Binary *b)
{
    uint8_t lType = b->left->Type();
    uint8_t rType = b->right->Type();
    TypeInfo info = {lType, b->op.type, rType};

    if(Operators.find(info) != Operators.end())
        return OperatorMap.at(info);

    return 0;
}

uint8_t TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    return es->exp->Type();
}


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

uint8_t ExprStmt::Type()
{
    return TypeChecker::TypeOfExprStmt(this);
}
