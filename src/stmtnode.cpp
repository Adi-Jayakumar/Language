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

DeclaredVar::DeclaredVar(TypeData _t, std::string _name, std::shared_ptr<Expr> _value, Token _loc)
{
    t = _t;
    name = _name;
    value = std::move(_value);
    loc = _loc;
}

Token DeclaredVar::Loc()
{
    return loc;
}

ArrayDecl::ArrayDecl(TypeData _elemType, std::string _name, std::vector<std::shared_ptr<Expr>> _init, Token _loc)
{
    elemType = _elemType;
    name = _name;
    init = _init;
    loc = _loc;
}

Token ArrayDecl::Loc()
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

FuncDecl::FuncDecl(TypeData _ret, std::string &_name, std::vector<Token> &_params, std::vector<std::shared_ptr<Stmt>> &_body, Token _loc)
{
    ret = _ret;
    name = _name;

    for (size_t i = 0; i < _params.size(); i++)
    {
        if (_params[i].type == TokenID::TYPENAME)
            argtypes.push_back(TypeNameMap[_params[i].literal]);
        else if (_params[i].type == TokenID::ARRAY)
        {
            i += 2;
            TypeData arrT = TypeNameMap[_params[i].literal];
            arrT.isArray = true;
            argtypes.push_back(arrT);
        }
        else if (_params[i].type == TokenID::IDEN)
            paramIdentifiers.push_back(_params[i].literal);
    }

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