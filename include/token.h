#pragma once
#include <cstdint>
#include <iostream>
#include <string>

enum class TokenID : uint8_t
{
    // literal values
    INT_L,
    DOUBLE_L,
    BOOL_L,

    // identifier
    IDEN,
    // type name
    TYPENAME,

    // arithmetic operators
    PLUS,
    MINUS,
    STAR,
    SLASH,

    // comparison operators
    GT,
    LT,
    GEQ,
    LEQ,

    // equality opreators
    EQ_EQ,
    BANG_EQ,

    // assignment operator
    EQ,

    // unary not opertion
    BANG,

    SEMI,

    OPEN_PAR,
    CLOSE_PAR,

    BEG,
    END,
};

struct Token
{
    TokenID type;
    std::string literal;
    size_t line;
};

bool IsLiteral(const Token &);

std::ostream &operator<<(std::ostream &out, const TokenID &t);
std::ostream &operator<<(std::ostream &out, const Token &t);