#pragma once
#include "nodeequal.h"

namespace NodeSubstituter
{
    SP<Expr> Substitute(SP<Expr> &tree, SP<Expr> &node, SP<Expr> &val);
    SP<Stmt> Substitute(SP<Stmt> &tree, SP<Expr> &node, SP<Expr> &val);
}