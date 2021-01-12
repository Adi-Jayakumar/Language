#include "stmtnode.h"

Stmt::~Stmt(){}

ExprStmt::ExprStmt(Expr* _exp, Token _loc)
{
    exp = _exp;
    loc = _loc;
}

ExprStmt::~ExprStmt()
{
    delete exp;
}

DeclaredVar::DeclaredVar(uint8_t _tId, std::string _name, Expr* _value, Token _loc)
{
    tId = _tId;
    name = _name;
    value = _value;
    loc = _loc;
}

DeclaredVar::~DeclaredVar()
{
    delete value;
}