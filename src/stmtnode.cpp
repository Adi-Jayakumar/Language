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