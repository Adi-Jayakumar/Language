#pragma once
#include "exprnode.h"

struct Stmt
{
    virtual void Print(std::ostream &out) = 0;
    virtual uint8_t Type() = 0;
    virtual ~Stmt() = 0;
};

std::ostream& operator<<(std::ostream &out, Stmt* s);

struct ExprStmt : Stmt
{
    Expr* exp;
    ExprStmt(Expr*);
    ~ExprStmt();
    
    void Print(std::ostream &out) override;
    uint8_t Type();
};