#include "exprnode.h"

Literal::Literal(Token val)
{
    loc = val;
    switch (DefaultTypeMap.at(val.type))
    {
    case 1:
    {
        typeID = 1;
        as.i = std::stoi(val.literal);
        break;
    }
    case 2:
    {
        typeID = 2;
        as.d = std::stod(val.literal);
        break;
    }
    case 3:
    {
        typeID = 3;
        if (val.literal == "true")
            as.b = true;
        else
            as.b = false;
    }
    }
}

Token Literal::Loc()
{
    return loc;
}

TypeID Literal::GetType()
{
    return typeID;
}

Unary::Unary(Token _op, std::shared_ptr<Expr> _right)
{
    op = _op;
    right = _right;
}

Token Unary::Loc()
{
    return op;
}

TypeID Unary::GetType()
{
    return typeID;
}

Binary::Binary(std::shared_ptr<Expr> _left, Token _op, std::shared_ptr<Expr> _right)
{
    left = _left;
    op = _op;
    right = _right;
}

Token Binary::Loc()
{
    return op;
}

TypeID Binary::GetType()
{
    return typeID;
}

Assign::Assign(std::shared_ptr<VarReference> _var, std::shared_ptr<Expr> _val, Token _loc)
{
    var = _var;
    val = _val;
    loc = _loc;
}

Token Assign::Loc()
{
    return loc;
}

TypeID Assign::GetType()
{
    return typeID;
}

VarReference::VarReference(Token _loc)
{
    loc = _loc;
    name = _loc.literal;
}
Token VarReference::Loc()
{
    return loc;
}

TypeID VarReference::GetType()
{
    return typeID;
}

FunctionCall::FunctionCall(std::string _name, std::vector<std::shared_ptr<Expr>> _args, Token _loc)
{
    name = _name;
    args = _args;
    loc = _loc;
}

Token FunctionCall::Loc()
{
    return loc;
}

TypeID FunctionCall::GetType()
{
    return typeID;
}

ArrayIndex::ArrayIndex(std::string _name, std::shared_ptr<Expr> _index, Token _loc)
{
    name = name;
    index = index;
    loc = _loc;
}

Token ArrayIndex::Loc()
{
    return loc;
}

TypeID ArrayIndex::GetType()
{
    return typeID;
}