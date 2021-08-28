#pragma once
#include "nodeequal.h"

namespace NodeSubstitution
{
    SP<Expr> Substitute(SP<Expr> &tree, SP<Expr> &node, SP<Expr> &val);
    SP<Stmt> Substitute(SP<Stmt> &tree, SP<Stmt> &node, SP<Stmt> &val);
}