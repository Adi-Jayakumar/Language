#include "stmtnode.h"

Stmt::~Stmt(){}

ExprStmt::ExprStmt(Expr* _exp)
{
    exp = _exp;
}

ExprStmt::~ExprStmt()
{
    delete exp;
}

DeclaredVar::DeclaredVar(uint8_t _tId, std::string _name, Expr* _value)
{
    tId = _tId;
    name = _name;
    value = _value;
}

DeclaredVar::~DeclaredVar()
{
    delete value;
}