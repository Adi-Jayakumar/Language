#pragma once
#include "maps.h"
#include "token.h"
#include <cstdint>
#include <unordered_map>

struct Expr
{
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual uint8_t Type() = 0;
    virtual ~Expr() = 0;
};

std::ostream &operator<<(std::ostream &out, Expr *e);

/*
    TypeIDs are represented as uint8_t with
    1 ---> int
    2 ---> double
    3 ---> bool
*/
struct Literal : Expr
{
    uint8_t typeID;
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
    Literal(Token);

    void Print(std::ostream &out) override;
    uint8_t Type();
};

struct Unary : Expr
{
    Token op;
    Expr *right;
    Unary(Token, Expr *);
    ~Unary();

    void Print(std::ostream &out) override;
    uint8_t Type();
};

struct Binary : Expr
{
    Expr *left;
    Token op;
    Expr *right;
    Binary(Expr *, Token, Expr *);
    ~Binary();

    void Print(std::ostream &out) override;
    uint8_t Type() override;
};

struct Assign : Expr
{
    std::string name;
    Expr *val;
    Assign(std::string, Expr *);
    ~Assign();

    void Print(std::ostream &out) override;
    uint8_t Type() override;
};

struct VarReference : Expr
{
    std::string name;
    VarReference(std::string);

    void Print(std::ostream &out) override;
    uint8_t Type() override;
};