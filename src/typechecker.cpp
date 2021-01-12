#include "typechecker.h"

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

void TypeChecker::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + "\n" + err);
    e.Dump();
}

//-----------------EXPRESSIONS---------------------//

TypeID TypeChecker::TypeOfLiteral(Literal *l)
{
    if (l == nullptr)
        return 0;
    return l->typeID;
}

TypeID TypeChecker::TypeOfUnary(Unary *u)
{
    if (u == nullptr || u->right == nullptr)
        return 0;
    TypeID opType = u->right->Type();
    TypeInfo info = {opType, u->op.type, 0};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);
    else
        TypeError(u->loc, "Cannot use operator: " + std::to_string(static_cast<uint8_t>(u->op.type)) + " on operand of type: " + std::to_string(opType));

    return ~0;
}

TypeID TypeChecker::TypeOfBinary(Binary *b)
{
    if (b == nullptr || b->left == nullptr || b->right == nullptr)
        return 0;
    TypeID lType = b->left->Type();
    TypeID rType = b->right->Type();
    TypeInfo info = {lType, b->op.type, rType};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);
    else
        TypeError(b->loc, "Cannot use operator: " + std::to_string(static_cast<uint8_t>(b->op.type)) + " on operands of type: " + std::to_string(lType) + " and: " + std::to_string(rType));
    return ~0;
}

TypeID TypeChecker::TypeOfAssign(Assign *a)
{
    if (VarNameType.find(a->name) == VarNameType.end())
        TypeError(a->loc, "Variable '" + a->name + "' has not been defined yet");
    else
    {
        TypeID targetType = VarNameType[a->name];
        TypeID valType = a->val->Type();
        if(targetType != valType)
            TypeError(a->loc, "Cannot assign a: " + std::to_string(valType) + " to variable: " + a->name + " of type: " + std::to_string(targetType));
    }
    return ~0;
}

TypeID TypeChecker::TypeOfVarReference(VarReference *vr)
{
    if (VarNameType.find(vr->name) == VarNameType.end())
        TypeError(vr->loc, "Variable '" + vr->name + "' has not been defined yet");
    return VarNameType[vr->name];
}

//------------------STATEMENTS---------------------//

TypeID TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    if (es == nullptr || es->exp == nullptr)
        return 0;
    return es->exp->Type();
}

TypeID TypeChecker::TypeOfDeclaredVar(DeclaredVar *v)
{
    if (v == nullptr || v->value == nullptr)
        return 0;

    TypeID vType = v->tId;
    TypeID vValType = v->value->Type();

    if (VarNameType.find(v->name) == VarNameType.end())
        VarNameType[v->name] = vType;

    if (vType == vValType)
        return vType;

    return ~0;
}

//-----------------EXPRESSIONS---------------------//

TypeID Literal::Type()
{
    return TypeChecker::TypeOfLiteral(this);
}

TypeID Unary::Type()
{
    return TypeChecker::TypeOfUnary(this);
}

TypeID Binary::Type()
{
    return TypeChecker::TypeOfBinary(this);
}

TypeID Assign::Type()
{
    return TypeChecker::TypeOfAssign(this);
}

TypeID VarReference::Type()
{
    return TypeChecker::TypeOfVarReference(this);
}

//------------------STATEMENTS---------------------//

TypeID ExprStmt::Type()
{
    return TypeChecker::TypeOfExprStmt(this);
}

TypeID DeclaredVar::Type()
{
    return TypeChecker::TypeOfDeclaredVar(this);
}