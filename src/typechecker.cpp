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

void TypeChecker::TypeCheck(std::shared_ptr<Stmt> &s)
{
    s->Type(*this);
}

TypeID TypeChecker::ResolveVariable(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name)
            return vars[i].type;
    }
    return 255;
}

void TypeChecker::CleanUpVariables()
{
    while(!vars.empty() && vars.back().depth == depth)
        vars.pop_back();
}

//-----------------EXPRESSIONS---------------------//

TypeID TypeChecker::TypeOfLiteral(Literal *l)
{
    if (l == nullptr)
        return 255;
    return l->typeID;
}

TypeID TypeChecker::TypeOfUnary(Unary *u)
{
    if (u == nullptr)
        return 0;
    if (u->right == nullptr)
        return 255;

    TypeID opType = u->right->Type(*this);
    TypeInfo info = {opType, u->op.type, 0};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);
    else
        TypeError(u->loc, "Cannot use operator: " + std::to_string(static_cast<uint8_t>(u->op.type)) + " on operand of type: " + std::to_string(opType));

    return 255;
}

TypeID TypeChecker::TypeOfBinary(Binary *b)
{
    if (b == nullptr || b->left == nullptr || b->right == nullptr)
        return 255;

    TypeID lType = b->left->Type(*this);
    TypeID rType = b->right->Type(*this);
    TypeInfo info = {lType, b->op.type, rType};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);
    else
        TypeError(b->loc, "Cannot use operator: " + std::to_string(static_cast<uint8_t>(b->op.type)) + " on operands of type: " + std::to_string(lType) + " and: " + std::to_string(rType));
    return 255;
}

TypeID TypeChecker::TypeOfAssign(Assign *a)
{
    TypeID varType = ResolveVariable(a->var->name);
    TypeID valType = a->val->Type(*this);

    if (varType == valType)
        return varType;
    else
        TypeError(a->loc, "Cannot assign value of type: " + std::to_string(valType) + " to variable: '" + a->var->name + "' of type: " + std::to_string(varType));
    return 255;
}

TypeID TypeChecker::TypeOfVarReference(VarReference *vr)
{
    TypeID type = ResolveVariable(vr->name);

    if (type == 255)
        TypeError(vr->loc, "Variable name: '" + vr->name + "' has not been defined before");

    return type;
}

//------------------STATEMENTS---------------------//

TypeID TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    if (es == nullptr || es->exp == nullptr)
        return 0;
    return es->exp->Type(*this);
}

TypeID TypeChecker::TypeOfDeclaredVar(DeclaredVar *v)
{
    if (v == nullptr)
        return 255;
    vars.push_back({v->tId, v->name, depth});
    if (v->value == nullptr)
        return v->tId;
    else
    {
        TypeID varType = v->tId;
        TypeID valType = v->value->Type(*this);

        if (valType == varType)
            return valType;
        else
            TypeError(v->loc, "Cannot assign value of type: " + std::to_string(valType) + " to variable: '" + v->name + "' of type: " + std::to_string(varType));
    }
    return 255;
}

TypeID TypeChecker::TypeOfBlock(Block *b)
{
    depth++;
    if(depth == UINT8_MAX)
        TypeError(b->loc, "Exceeded maximum number of nested blocks: " + std::to_string(UINT8_MAX));
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s->Type(*this);
    CleanUpVariables();
    depth--;
    return 0;
}

//-----------------EXPRESSIONS---------------------//

TypeID Literal::Type(TypeChecker &t)
{
    return t.TypeOfLiteral(this);
}

TypeID Unary::Type(TypeChecker &t)
{
    return t.TypeOfUnary(this);
}

TypeID Binary::Type(TypeChecker &t)
{
    return t.TypeOfBinary(this);
}

TypeID Assign::Type(TypeChecker &t)
{
    return t.TypeOfAssign(this);
}

TypeID VarReference::Type(TypeChecker &t)
{
    return t.TypeOfVarReference(this);
}

//------------------STATEMENTS---------------------//

TypeID ExprStmt::Type(TypeChecker &t)
{
    return t.TypeOfExprStmt(this);
}

TypeID DeclaredVar::Type(TypeChecker &t)
{
    return t.TypeOfDeclaredVar(this);
}

TypeID Block::Type(TypeChecker &t)
{
    return t.TypeOfBlock(this);
}