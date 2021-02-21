#include "exprnode.h"

Literal::Literal(Token val)
{
    loc = val;
    switch (DefaultTypeMap.at(val.type))
    {
    case 1:
    {
        t = {false, 1};
        as.i = std::stoi(val.literal);
        break;
    }
    case 2:
    {
        t = {false, 2};
        as.d = std::stod(val.literal);
        break;
    }
    case 3:
    {
        t = {false, 3};
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

TypeData Literal::GetType()
{
    return t;
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

TypeData Unary::GetType()
{
    return t;
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

TypeData Binary::GetType()
{
    return t;
}

Assign::Assign(std::shared_ptr<Expr> _target, std::shared_ptr<Expr> _val, Token _loc)
{
    target = _target;
    val = _val;
    loc = _loc;
}

Token Assign::Loc()
{
    return loc;
}

TypeData Assign::GetType()
{
    return t;
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

TypeData VarReference::GetType()
{
    return t;
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

TypeData FunctionCall::GetType()
{
    return t;
}

ArrayIndex::ArrayIndex(std::string _name, std::shared_ptr<Expr> _index, Token _loc)
{
    name = _name;
    index = _index;
    loc = _loc;
}

Token ArrayIndex::Loc()
{
    return loc;
}

TypeData ArrayIndex::GetType()
{
    return t;
}

InlineArray::InlineArray(size_t _size, std::vector<std::shared_ptr<Expr>> _init, Token _loc)
{
    size = _size;
    init = _init;
    loc = _loc;
}

Token InlineArray::Loc()
{
    return loc;
}

TypeData InlineArray::GetType()
{
    return t;
}

DynamicAllocArray::DynamicAllocArray(TypeData _t, std::shared_ptr<Expr> _size, Token _loc)
{
    t = _t;
    size = _size;
    loc = _loc;
}

Token DynamicAllocArray::Loc()
{
    return loc;
}

TypeData DynamicAllocArray::GetType()
{
    return t;
}