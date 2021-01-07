#pragma once
#include <iostream>
#include <string>

enum class TokenID
{
    DOUBLE_L,

    PLUS,
    MINUS,
    STAR,
    SLASH,

    OPEN_PAR,
    CLOSE_PAR,
    
    BEG,
    END
};

struct Token
{
    TokenID type;
    std::string literal;
    size_t line;
};

std::ostream &operator<<(std::ostream &out, const TokenID &t);
std::ostream &operator<<(std::ostream &out, const Token &t);