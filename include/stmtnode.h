#pragma once
#include "exprnode.h"
#include <vector>

struct Stmt : std::enable_shared_from_this<Stmt>
{
    Token loc;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeID Type(TypeChecker &t) = 0;
    // compiles the node - implemented in compiler.cpp
    virtual void NodeCompile(Chunk &c) = 0;
};

std::ostream &operator<<(std::ostream &out, Stmt *s);

struct ExprStmt : Stmt
{
    std::shared_ptr<Expr> exp;
    ExprStmt(std::shared_ptr<Expr>, Token);

    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
};

struct DeclaredVar : Stmt
{
    TypeID tId;
    std::string name;
    std::shared_ptr<Expr> value;
    DeclaredVar(TypeID, std::string, std::shared_ptr<Expr> , Token);

    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
};

struct Block : Stmt
{
    uint8_t depth;
    std::vector<std::shared_ptr<Stmt>> stmts;
    Block(uint8_t, Token);

    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
};

struct IfStmt : Stmt
{
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
    IfStmt(std::shared_ptr<Expr> , std::shared_ptr<Stmt>, std::shared_ptr<Stmt> , Token);
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
};