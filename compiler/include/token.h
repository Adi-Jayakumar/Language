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
    STRING_L,
    CHAR_L,

    // identifier
    IDEN,
    TYPENAME,
    FUNC,
    RETURN,
    BREAK,
    COMMA,
    DOT,
    NULL_T,

    ARRAY,
    WHILE,
    STRUCT,
    COLON,

    IMPORT,
    FROM,

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

    // opening and closing verifcation statements
    OPEN_VER,
    CLOSE_VER,
    // result of calling a function used in the verifier's post conditions
    RESULT,

    OPEN_BRACE,
    CLOSE_BRACE,

    OPEN_SQ,
    CLOSE_SQ,

    IF,
    ELSE,

    BEG,
    END,
};

struct Token
{
    TokenID type;
    std::string literal;
    size_t line;

    Token() = default;
    Token(TokenID _type, std::string _literal, size_t _line) : type(_type), literal(_literal), line(_line){};
};

bool IsLiteral(const Token &);

std::string ToString(const TokenID &token);

std::ostream &operator<<(std::ostream &out, const TokenID &t);
std::ostream &operator<<(std::ostream &out, const Token &t);