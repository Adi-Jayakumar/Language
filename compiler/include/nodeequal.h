#pragma once
#include "stmtnode.h"

template <class T>
using SP = std::shared_ptr<T>;

namespace NodeEqual
{
    bool Equal(const SP<Expr> &left, const SP<Expr> &right);
    bool Equal(const SP<Stmt> &left, const SP<Stmt> &right);
}