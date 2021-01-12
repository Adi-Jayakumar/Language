#pragma once
#include "exprnode.h"

struct Stmt
{
    Token loc;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeID Type() = 0;
    virtual ~Stmt() = 0;
};

std::ostream &operator<<(std::ostream &out, Stmt *s);

struct ExprStmt : Stmt
{
    Expr *exp;
    ExprStmt(Expr *, Token);
    ~ExprStmt();

    void Print(std::ostream &out) override;
    TypeID Type();
};

struct DeclaredVar : Stmt
{
    TypeID tId;
    std::string name;
    Expr *value;
    DeclaredVar(TypeID, std::string, Expr *, Token);
    ~DeclaredVar();

    void Print(std::ostream &out) override;
    TypeID Type() override;
};