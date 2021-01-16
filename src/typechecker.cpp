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

TypeID TypeChecker::ResolveLocal(std::string &name)
{
    for (size_t i = Variables.size() - 1; (int)i >= 0; i--)
    {
        if (Variables[i].name == name)
            return Variables[i].type;
    }
    return ~0;
}

//-----------------EXPRESSIONS---------------------//

TypeID TypeChecker::TypeOfLiteral(Literal *l)
{
    if (l == nullptr)
        return ~0;
    return l->typeID;
}

TypeID TypeChecker::TypeOfUnary(Unary *u)
{
    if (u == nullptr)
        return 0;
    if (u->right == nullptr)
        return ~0;

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
        return ~0;

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
    TypeID varType = ResolveLocal(a->var->name);
    TypeID valType = a->val->Type();

    if (varType == valType)
        return varType;
    else
        TypeError(a->loc, "Cannot assign value of type: " + std::to_string(valType) + " to variable: '" + a->var->name + "' of type: " + std::to_string(varType));
    return ~0;
}

TypeID TypeChecker::TypeOfVarReference(VarReference *vr)
{
    TypeID type = ResolveLocal(vr->name);

    if (type == ~0)
        TypeError(vr->loc, "Variable name: '" + vr->name + "' has not been defined before");

    return type;
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
    if (v == nullptr)
        return ~0;
    Variables.push_back({v->name, v->tId});
    if (v->value == nullptr)
        return v->tId;
    else
    {
        TypeID varType = v->tId;
        TypeID valType = v->value->Type();

        if (valType == varType)
            return valType;
        else
            TypeError(v->loc, "Cannot assign value of type: " + std::to_string(valType) + " to variable: '" + v->name + "' of type: " + std::to_string(varType));
    }
    return ~0;
}

TypeID TypeChecker::TypeOfBlock(Block *b)
{
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s->Type();
    return 0;
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

TypeID Block::Type()
{
    return TypeChecker::TypeOfBlock(this);
}