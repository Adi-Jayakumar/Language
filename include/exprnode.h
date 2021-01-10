#pragma once
#include "token.h"
#include <cstdint>
#include <unordered_map>

static const std::unordered_map<TokenID, uint8_t> DefaultTypeMap{
    {TokenID::INT_L, 1},
    {TokenID::DOUBLE_L, 2},
    {TokenID::BOOL_L, 3}};

struct Expr
{
    virtual void Print(std::ostream &out) = 0;
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
    Literal(Token val);

    void Print(std::ostream &out) override;
    uint8_t Type();
};

struct Unary : Expr
{
    Token op;
    Expr *right;
    Unary(Token _op, Expr *_right);
    ~Unary();
    
    void Print(std::ostream &out) override;
    uint8_t Type();
};

struct Binary : Expr
{
    Expr *left;
    Token op;
    Expr *right;
    Binary(Expr *_left, Token _op, Expr *_right);
    ~Binary();
    
    void Print(std::ostream &out) override;
    uint8_t Type();
};
