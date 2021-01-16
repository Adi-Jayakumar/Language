#pragma once
#include "maps.h"
#include "token.h"
#include <memory>

struct Chunk;

struct Expr
{
    Token loc;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeID Type() = 0;
    // compiles the node - implmented in compiler.cpp
    virtual void NodeCompile(Chunk &c) = 0;
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
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
    Literal(Token);
    // ~Literal() override = default;

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct Unary : Expr
{
    Token op;
    std::shared_ptr<Expr> right;
    Unary(Token, std::shared_ptr<Expr>);
    // ~Unary() override = default;

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct Binary : Expr
{
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
    Binary(std::shared_ptr<Expr>, Token, std::shared_ptr<Expr>);
    // ~Binary() override = default;

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct VarReference : Expr
{
    std::string name;
    VarReference(Token);
    // ~VarReference() override = default;

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct Assign : Expr
{
    std::shared_ptr<VarReference> var;
    std::shared_ptr<Expr> val;
    Assign(std::shared_ptr<VarReference>, std::shared_ptr<Expr>, Token);
    // ~Assign() override = default;

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};