#pragma once
#include "exprnode.h"

struct Stmt
{
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual uint8_t Type() = 0;
    virtual ~Stmt() = 0;
};

std::ostream &operator<<(std::ostream &out, Stmt *s);

struct ExprStmt : Stmt
{
    Expr *exp;
    ExprStmt(Expr *);
    ~ExprStmt();

    void Print(std::ostream &out) override;
    uint8_t Type();
};

struct DeclaredVar : Stmt
{
    uint8_t tId;
    std::string name;
    Expr *value;
    DeclaredVar(uint8_t, std::string, Expr *);
    ~DeclaredVar();

    void Print(std::ostream &out) override;
    uint8_t Type() override;
};