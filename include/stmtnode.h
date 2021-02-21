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
    virtual TypeData Type(TypeChecker &t) = 0;
    // determines whether all the node's code paths return the appropriate type
    virtual bool DoesReturn(TypeData ret) = 0;
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
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};

struct DeclaredVar : Stmt
{
    TypeData t;
    std::string name;
    std::shared_ptr<Expr> value;
    DeclaredVar(TypeData, std::string, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};

struct ArrayDecl : Stmt
{
    TypeData elemType;
    std::string name;
    size_t size;
    std::vector<std::shared_ptr<Expr>> init;

    ArrayDecl(TypeData, std::string, std::vector<std::shared_ptr<Expr>>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};

struct Block : Stmt
{
    uint8_t depth;
    std::vector<std::shared_ptr<Stmt>> stmts;
    Block(uint8_t, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
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
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};

struct WhileStmt : Stmt
{
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> body;
    Token loc;

    WhileStmt(std::shared_ptr<Expr>, std::shared_ptr<Stmt>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};

struct FuncDecl : Stmt
{
    TypeData ret;
    std::string name;
    std::vector<TypeData> argtypes;
    std::vector<std::string> paramIdentifiers;
    std::vector<std::shared_ptr<Stmt>> body;

    FuncDecl(TypeData, std::string &, std::vector<TypeData> &, std::vector<std::string> &, std::vector<std::shared_ptr<Stmt>> &, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};

struct Return : Stmt
{
    std::shared_ptr<Expr> retVal;
    Return(std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    bool DoesReturn(TypeData ret) override;
    void NodeCompile(Compiler &c) override;
};