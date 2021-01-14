#pragma once
#include "maps.h"
#include "token.h"
#include <cstdint>
#include <unordered_map>

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
    virtual ~Expr() = 0;
};

std::ostream &operator<<(std::ostream &out, Expr *e);

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

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct Unary : Expr
{
    Token op;
    Expr *right;
    Unary(Token, Expr *);
    ~Unary();

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct Binary : Expr
{
    Expr *left;
    Token op;
    Expr *right;
    Binary(Expr *, Token, Expr *);
    ~Binary();

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct Assign : Expr
{
    std::string name;
    Expr *val;
    Assign(std::string, Expr *, Token);
    ~Assign();

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};

struct VarReference : Expr
{
    std::string name;
    VarReference(Token);

    void Print(std::ostream &out) override;
    TypeID Type() override;
    void NodeCompile(Chunk &c) override;
};