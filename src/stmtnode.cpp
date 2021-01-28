#include "stmtnode.h"

ExprStmt::ExprStmt(std::shared_ptr<Expr> _exp, Token _loc)
{
    exp = std::move(_exp);
    loc = _loc;
}

Token ExprStmt::Loc()
{
    return loc;
}

DeclaredVar::DeclaredVar(TypeID _tId, std::string _name, std::shared_ptr<Expr> _value, Token _loc)
{
    tId = _tId;
    name = _name;
    value = std::move(_value);
    loc = _loc;
}

Token DeclaredVar::Loc()
{
    return loc;
}

Block::Block(uint8_t _depth, Token _loc)
{
    depth = _depth;
    loc = _loc;
}

Token Block::Loc()
{
    return loc;
}

IfStmt::IfStmt(std::shared_ptr<Expr> _cond, std::shared_ptr<Stmt> _thenBranch, std::shared_ptr<Stmt> _elseBranch, Token _loc)
{
    cond = _cond;
    thenBranch = _thenBranch;
    elseBranch = _elseBranch;
    loc = _loc;
}

Token IfStmt::Loc()
{
    return loc;
}

WhileStmt::WhileStmt(std::shared_ptr<Expr> _cond, std::shared_ptr<Stmt> _body, Token _loc)
{
    cond = _cond;
    body = _body;
    loc = _loc;
}

Token WhileStmt::Loc()
{
    return loc;
}

FuncDecl::FuncDecl(TypeID _ret, std::string &_name, std::vector<Token> &_params, std::vector<std::shared_ptr<Stmt>> &_body, Token _loc)
{
    ret = _ret;
    name = _name;
    params = _params;
    body = _body;
    loc = _loc;
}

Token FuncDecl::Loc()
{
    return loc;
}

Return::Return(std::shared_ptr<Expr> _retVal, Token _loc)
{
    retVal = _retVal;
    loc = _loc;
}

Token Return::Loc()
{
    return loc;
}