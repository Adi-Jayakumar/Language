#pragma once
#include "common.h"
#include "token.h"
#include <memory>

struct Chunk;
struct TypeChecker;

struct Expr
{
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeID Type(TypeChecker &t) = 0;
    // compiles the node - implmented in compiler.cpp
    virtual void NodeCompile(Chunk &c) = 0;
    // virtual bool IsTruthy() = 0;
    // virtual ~Expr() = 0;
};

std::ostream &operator<<(std::ostream &out, std::shared_ptr<Expr> &e);

/*
    TypeIDs are represented as TypeID with
    1 ---> int
    2 ---> double
    3 ---> bool
*/
struct Literal : Expr
{
    TypeID typeID;
    Token loc;
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
    Literal(Token);
    // ~Literal() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
    // bool IsTruthy() override;
};

struct Unary : Expr
{
    Token op;
    std::shared_ptr<Expr> right;
    Unary(Token, std::shared_ptr<Expr>);
    // ~Unary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
    // bool IsTruthy() override;
};

struct Binary : Expr
{
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
    Binary(std::shared_ptr<Expr>, Token, std::shared_ptr<Expr>);
    // ~Binary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
    // bool IsTruthy() override;
};

struct VarReference : Expr
{
    Token loc;
    std::string name;
    VarReference(Token);
    // ~VarReference() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
    // bool IsTruthy() override;
};

struct Assign : Expr
{
    Token loc;
    std::shared_ptr<VarReference> var;
    std::shared_ptr<Expr> val;
    Assign(std::shared_ptr<VarReference>, std::shared_ptr<Expr>, Token);
    // ~Assign() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
    // bool IsTruthy() override;
};

struct FunctionCall : Expr
{
    Token loc;
    std::string name;
    std::vector<std::shared_ptr<Expr>> args;
    FunctionCall(std::string, std::vector<std::shared_ptr<Expr>> &, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    void NodeCompile(Chunk &c) override;
};