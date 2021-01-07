#pragma once
#include <cstdint>
#include <iostream>
#include <string>

enum class TokenID : uint8_t
{
    INT_L,
    DOUBLE_L,
    BOOL_L,

    IDEN,

    PLUS,
    MINUS,
    STAR,
    SLASH,

    GT,
    LT,
    GEQ,
    LEQ,

    EQ_EQ,
    BANG_EQ,

    EQ,
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