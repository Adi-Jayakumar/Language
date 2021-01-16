#include "stmtnode.h"

ExprStmt::ExprStmt(std::shared_ptr<Expr> &_exp, Token _loc)
{
    exp = std::move(_exp);
    loc = _loc;
}

DeclaredVar::DeclaredVar(uint8_t _tId, std::string _name, std::shared_ptr<Expr> &_value, Token _loc)
{
    tId = _tId;
    name = _name;
    value = std::move(_value);
    loc = _loc;
}

Block::Block(uint8_t _depth)
{
    depth = _depth;
}