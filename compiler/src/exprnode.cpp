#include "exprnode.h"

Literal::Literal(Token val)
{
    loc = val;
    TypeID type = DefaultTypeMap.at(val.type);
    t = {false, type};
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

ArrayIndex::ArrayIndex(std::shared_ptr<Expr> _name, std::shared_ptr<Expr> _index, Token _loc)
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

BracedInitialiser::BracedInitialiser(size_t _size, std::vector<std::shared_ptr<Expr>> _init, Token _loc)
{
    size = _size;
    init = _init;
    loc = _loc;
}

Token BracedInitialiser::Loc()
{
    return loc;
}

TypeData BracedInitialiser::GetType()
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

FieldAccess::FieldAccess(std::shared_ptr<Expr> _accessor, std::shared_ptr<Expr> _accessee, Token _loc)
{
    accessor = _accessor;
    accessee = _accessee;
    loc = _loc;
}

Token FieldAccess::Loc()
{
    return loc;
}

TypeData FieldAccess::GetType()
{
    return t;
}

TypeCast::TypeCast(TypeData _type, std::shared_ptr<Expr> _arg, Token _loc)
{
    type = _type;
    arg = _arg;
    loc = _loc;
}

Token TypeCast::Loc()
{
    return loc;
}

TypeData TypeCast::GetType()
{
    return t;
}