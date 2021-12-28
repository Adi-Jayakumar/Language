#pragma once
#include <cstdint>
#include <iostream>
#include <string>

using token_int_type = uint8_t;

enum class TokenID : token_int_type
{
    // literal values
    INT_L,
    DOUBLE_L,
    BOOL_L,
    STRING_L,
    CHAR_L,

    // identifier
    IDEN,
    // an actual typename
    TYPENAME,
    // the keyword 'typename'
    TYPENAME_KW,
    FUNC,
    CAST,
    // the keyword 'template'
    TEMPLATE,

    THROW,
    TRY,
    CATCH,

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

    // boolean operators
    AND_AND,
    OR_OR,

    // assignment operator
    EQ,

    // unary not opertion
    BANG,

    SEMI,

    OPEN_PAR,
    CLOSE_PAR,

    // opening and closing verifcation statements
    OPEN_VER,  // (|
    CLOSE_VER, // |)
    // result of calling a function used in the verifier's post conditions
    RESULT,
    // 'Sequence' keyword, used in verification conditions
    SEQUENCE,

    OPEN_BRACE,
    CLOSE_BRACE,

    OPEN_SQ,
    CLOSE_SQ,

    IF,
    ELSE,

    BEG,
    END,
};

// TODO Implement a location struct containing line, col, and file_name and integrate with AST nodes
struct Token
{
    TokenID type;
    std::string literal;
    size_t line;
    size_t col;
    // std::string file_name

    Token() = default;
    Token(const TokenID _type, const std::string _literal, const size_t _line, const size_t _col) : type(_type), literal(_literal), line(_line), col(_col){};
};

bool IsLiteral(const Token &);

std::string ToString(const TokenID &token);

std::ostream &operator<<(std::ostream &out, const TokenID &t);
std::ostream &operator<<(std::ostream &out, const Token &t);

inline bool CheckBinaryOperator(const TokenID &tok)
{
    return tok == TokenID::PLUS ||
           tok == TokenID::MINUS ||
           tok == TokenID::STAR ||
           tok == TokenID::SLASH ||
           tok == TokenID::GT ||
           tok == TokenID::LT ||
           tok == TokenID::GEQ ||
           tok == TokenID::LEQ ||
           tok == TokenID::EQ_EQ ||
           tok == TokenID::BANG_EQ ||
           tok == TokenID::AND_AND ||
           tok == TokenID::OR_OR;
}