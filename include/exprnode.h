#pragma once
#include "token.h"

struct Expr
{
    virtual void Print(std::ostream &out) = 0;
    virtual ~Expr() = 0;
};

std::ostream &operator<<(std::ostream &out, Expr *e);

struct Literal : Expr
{
    // will change when more types are added
    double val;
    Literal(double _val);

    void Print(std::ostream &out) override;
};

struct Unary : Expr
{
    Token op;
    Expr *right;
    Unary(Token _op, Expr *_right);
    ~Unary();
    
    void Print(std::ostream &out) override;
};

struct Grouping : Expr
{
    Expr *exp;
    Grouping(Expr *_exp);
    ~Grouping();
    
    void Print(std::ostream &out) override;
};

struct Binary : Expr
{
    Expr *left;
    Token op;
    Expr *right;
    Binary(Expr *_left, Token _op, Expr *_right);
    ~Binary();
    
    void Print(std::ostream &out) override;
};
