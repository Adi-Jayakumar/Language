#pragma once
#include "exprnode.h"
#include <vector>

struct Stmt
{
    Token loc;
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeID Type(TypeChecker &t) = 0;
    // compiles the node - implemented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
};

std::ostream &operator<<(std::ostream &out, Stmt *s);

struct ExprStmt : Stmt
{
    std::shared_ptr<Expr> exp;
    ExprStmt(std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Compiler &c) override;
};

struct DeclaredVar : Stmt
{
    TypeID tId;
    std::string name;
    std::shared_ptr<Expr> value;
    DeclaredVar(TypeID, std::string, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Compiler &c) override;
};

struct Block : Stmt
{
    uint8_t depth;
    std::vector<std::shared_ptr<Stmt>> stmts;
    Block(uint8_t, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Compiler &c) override;
};

struct IfStmt : Stmt
{
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
    IfStmt(std::shared_ptr<Expr>, std::shared_ptr<Stmt>, std::shared_ptr<Stmt>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Compiler &c) override;
};

struct FuncDecl : Stmt
{
    TypeID ret;
    std::string name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body;

    FuncDecl(TypeID, std::string &, std::vector<Token> &, std::vector<std::shared_ptr<Stmt>> &, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Compiler &c) override;
};

struct Return : Stmt
{
    std::shared_ptr<Expr> retVal;
    Return(std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Compiler &c) override;
};