#include "exprnode.h"

Literal::Literal(Token val)
{
    kind = ExprKind::LITERAL;
    loc = val;
    TypeID type = DefaultTypeMap.at(val.type);
    t = {false, type};
}

Token Literal::Loc()
{
    return loc;
}

Unary::Unary(Token _op, std::shared_ptr<Expr> _right)
{
    kind = ExprKind::UNARY;
    op = _op;
    right = _right;
}

Token Unary::Loc()
{
    return op;
}

Binary::Binary(std::shared_ptr<Expr> _left, Token _op, std::shared_ptr<Expr> _right)
{
    kind = ExprKind::BINARY;
    left = _left;
    op = _op;
    right = _right;
}

Token Binary::Loc()
{
    return op;
}

Assign::Assign(std::shared_ptr<Expr> _target, std::shared_ptr<Expr> _val, Token _loc)
{
    kind = ExprKind::ASSIGN;
    target = _target;
    val = _val;
    loc = _loc;
}

Token Assign::Loc()
{
    return loc;
}

VarReference::VarReference(Token _loc)
{
    kind = ExprKind::VAR_REFERENCE;
    loc = _loc;
    name = _loc.literal;
}
Token VarReference::Loc()
{
    return loc;
}

FunctionCall::FunctionCall(std::string _name, std::vector<TypeData> _templates, std::vector<std::shared_ptr<Expr>> _args, Token _loc)
{
    kind = ExprKind::FUNCTION_CALL;
    name = _name;
    templates = _templates;
    args = _args;
    loc = _loc;
}

Token FunctionCall::Loc()
{
    return loc;
}

ArrayIndex::ArrayIndex(std::shared_ptr<Expr> _name, std::shared_ptr<Expr> _index, Token _loc)
{
    kind = ExprKind::ARRAY_INDEX;
    name = _name;
    index = _index;
    loc = _loc;
}

Token ArrayIndex::Loc()
{
    return loc;
}

BracedInitialiser::BracedInitialiser(size_t _size, std::vector<std::shared_ptr<Expr>> _init, Token _loc)
{
    kind = ExprKind::BRACED_INITIALISER;
    size = _size;
    init = _init;
    loc = _loc;
}

Token BracedInitialiser::Loc()
{
    return loc;
}

DynamicAllocArray::DynamicAllocArray(TypeData _t, std::shared_ptr<Expr> _size, Token _loc)
{
    kind = ExprKind::DYNAMIC_ALLOC_ARRAY;
    t = _t;
    size = _size;
    loc = _loc;
}

Token DynamicAllocArray::Loc()
{
    return loc;
}

FieldAccess::FieldAccess(std::shared_ptr<Expr> _accessor, std::shared_ptr<Expr> _accessee, Token _loc)
{
    kind = ExprKind::FIELD_ACCESS;
    accessor = _accessor;
    accessee = _accessee;
    loc = _loc;
}

Token FieldAccess::Loc()
{
    return loc;
}

TypeCast::TypeCast(TypeData _type, std::shared_ptr<Expr> _arg, Token _loc)
{
    kind = ExprKind::TYPE_CAST;
    type = _type;
    arg = _arg;
    loc = _loc;
}

Token TypeCast::Loc()
{
    return loc;
}