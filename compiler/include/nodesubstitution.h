#pragma once
#include "nodeequal.h"

namespace NodeSubstitution
{
    SP<Expr> Substitute(SP<Expr> &tree, SP<Expr> &node, SP<Expr> &val);
    void Substitute(SP<Stmt> &tree, SP<Expr> &node, SP<Expr> &val);
}