#include "exprnode.h"

Literal::Literal(double _val)
{
    val = _val;
}

Unary::Unary(Token _op, Expr* _right)
{
    op = _op;
    right = _right;
}

Grouping::Grouping(Expr* _exp)
{
    exp = _exp;
}

Binary::Binary(Expr* _left, Token _op, Expr* _right)
{
    left = _left;
    op = _op;
    right = _right;
}
