#pragma once
#include "stmtnode.h"
#include <unordered_set>
#include <unordered_map>

struct TypeInfo
{
    uint8_t left;
    TokenID t;
    uint8_t right;
};

bool operator==(const TypeInfo &l, const TypeInfo &r);

struct TypeInfoHasher
{
    size_t operator()(const TypeInfo &t) const
    {
        size_t l = static_cast<size_t>(t.left);
        size_t r = static_cast<size_t>(t.right);
        size_t tok = static_cast<size_t>(t.t);
        return (tok << 16) | (r << 8) | (l);
    }
};

static const std::unordered_set<TypeInfo, TypeInfoHasher> Operators
    {{1, TokenID::PLUS, 1},
     {1, TokenID::PLUS, 2},
     {2, TokenID::PLUS, 1},
     {2, TokenID::PLUS, 2},
     {1, TokenID::MINUS, 1},
     {1, TokenID::MINUS, 2},
     {2, TokenID::MINUS, 1},
     {2, TokenID::MINUS, 2},
     {1, TokenID::STAR, 1},
     {1, TokenID::STAR, 2},
     {2, TokenID::STAR, 1},
     {2, TokenID::STAR, 2},
     {1, TokenID::SLASH, 1},
     {1, TokenID::SLASH, 2},
     {2, TokenID::SLASH, 1},
     {2, TokenID::SLASH, 2},
     {1, TokenID::GT, 1},
     {1, TokenID::GT, 2},
     {2, TokenID::GT, 1},
     {2, TokenID::GT, 2},
     {1, TokenID::LT, 1},
     {1, TokenID::LT, 2},
     {2, TokenID::LT, 1},
     {2, TokenID::LT, 2},
     {1, TokenID::GEQ, 1},
     {1, TokenID::GEQ, 2},
     {2, TokenID::GEQ, 1},
     {2, TokenID::GEQ, 2},
     {1, TokenID::LEQ, 1},
     {1, TokenID::LEQ, 2},
     {2, TokenID::LEQ, 1},
     {2, TokenID::LEQ, 2},
     {1, TokenID::EQ_EQ, 1},
     {1, TokenID::EQ_EQ, 2},
     {2, TokenID::EQ_EQ, 1},
     {2, TokenID::EQ_EQ, 2},
     {1, TokenID::BANG_EQ, 1},
     {1, TokenID::BANG_EQ, 2},
     {2, TokenID::BANG_EQ, 1},
     {2, TokenID::BANG_EQ, 2},
     {0, TokenID::BANG, 3}};

static const std::unordered_map<TypeInfo, uint8_t, TypeInfoHasher>
    OperatorMap{
        {{1, TokenID::PLUS, 1}, 1},
        {{1, TokenID::PLUS, 2}, 2},
        {{2, TokenID::PLUS, 1}, 2},
        {{2, TokenID::PLUS, 2}, 2},
        {{1, TokenID::MINUS, 1}, 1},
        {{1, TokenID::MINUS, 2}, 2},
        {{2, TokenID::MINUS, 1}, 2},
        {{2, TokenID::MINUS, 2}, 2},
        {{1, TokenID::STAR, 1}, 1},
        {{1, TokenID::STAR, 2}, 2},
        {{2, TokenID::STAR, 1}, 2},
        {{2, TokenID::STAR, 2}, 2},
        {{1, TokenID::SLASH, 1}, 1},
        {{1, TokenID::SLASH, 2}, 2},
        {{2, TokenID::SLASH, 1}, 2},
        {{2, TokenID::SLASH, 2}, 2},
        {{1, TokenID::GT, 1}, 3},
        {{1, TokenID::GT, 2}, 3},
        {{2, TokenID::GT, 1}, 3},
        {{2, TokenID::GT, 2}, 3},
        {{1, TokenID::LT, 1}, 3},
        {{1, TokenID::LT, 2}, 3},
        {{2, TokenID::LT, 1}, 3},
        {{2, TokenID::LT, 2}, 3},
        {{1, TokenID::GEQ, 1}, 3},
        {{1, TokenID::GEQ, 2}, 3},
        {{2, TokenID::GEQ, 1}, 3},
        {{2, TokenID::GEQ, 2}, 3},
        {{1, TokenID::LEQ, 1}, 3},
        {{1, TokenID::LEQ, 2}, 3},
        {{2, TokenID::LEQ, 1}, 3},
        {{2, TokenID::LEQ, 2}, 3},
        {{1, TokenID::EQ_EQ, 1}, 3},
        {{1, TokenID::EQ_EQ, 2}, 3},
        {{2, TokenID::EQ_EQ, 1}, 3},
        {{2, TokenID::EQ_EQ, 2}, 3},
        {{1, TokenID::BANG_EQ, 1}, 3},
        {{1, TokenID::BANG_EQ, 2}, 3},
        {{2, TokenID::BANG_EQ, 1}, 3},
        {{2, TokenID::BANG_EQ, 2}, 3},
        {{0, TokenID::BANG, 3}, 3},
    };

namespace TypeChecker
{
    // expression typechecking
    uint8_t TypeOfLiteral(Literal *l);
    uint8_t TypeOfUnary(Unary *u);
    uint8_t TypeOfBinary(Binary *b);

    // statment typechecking
    uint8_t TypeOfExprStmt(ExprStmt *es);
} // namespace TypeChecker