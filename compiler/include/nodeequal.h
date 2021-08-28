#pragma once
#include "stmtnode.h"

namespace NodeEqual
{
    bool Equal(const SP<Expr> &left, const SP<Expr> &right);
    bool Equal(const SP<Stmt> &left, const SP<Stmt> &right);
}