#include "exprnode.h"

Expr::~Expr(){}

Literal::Literal(double _val)
{
    val = _val;
}

Unary::Unary(Token _op, Expr* _right)
{
    op = _op;
    right = _right;
}

Unary::~Unary()
{
    delete right;
}

Grouping::Grouping(Expr* _exp)
{
    exp = _exp;
}

Grouping::~Grouping()
{
    delete exp;
}

Binary::Binary(Expr* _left, Token _op, Expr* _right)
{
    left = _left;
    op = _op;
    right = _right;
}

Binary::~Binary()
{
    delete left;
    delete right;
}