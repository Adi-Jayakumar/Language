#pragma once
#include "token.h"

struct Expr{};

struct Literal
{
    // will change when more types are added
    double val;
    Literal(double _val);
};

struct Unary : Expr
{
    Token op;
    Expr* right;
    Unary(Token _op, Expr *_right);
};

struct Grouping : Expr
{
    Expr* exp;
    Grouping(Expr* _exp);
};

struct Binary : Expr
{
    Expr* left;
    Token op;
    Expr* right;
    Binary(Expr* _left, Token _op, Expr *_right);
};