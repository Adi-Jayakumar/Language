#pragma once
#include "token.h"
#include <unordered_map>
#include <vector>

typedef uint8_t TypeID;

// map from literal TokenIDs to
static const std::unordered_map<TokenID, TypeID> DefaultTypeMap{
    {TokenID::INT_L, 1},
    {TokenID::DOUBLE_L, 2},
    {TokenID::BOOL_L, 3}};

// map from string typenames to uint8_t type ids.
static std::unordered_map<std::string, TypeID> TypeNameMap{{"int", 1}, {"double", 2}, {"bool", 3}};

struct TypeInfo
{
    TypeID left;
    TokenID t;
    TypeID right;
};

bool operator==(const TypeInfo &l, const TypeInfo &r);

struct TypeInfoHasher
{
    size_t operator()(const TypeInfo &t) const
    {
        size_t l = static_cast<size_t>(t.left);
        size_t r = static_cast<size_t>(t.right);
        size_t tok = static_cast<size_t>(t.t);
        return (l << 16) | (r << 8) | (tok);
    }
};

static const std::unordered_map<TypeInfo, TypeID, TypeInfoHasher>
    OperatorMap{
        // binary plus
        {{1, TokenID::PLUS, 1}, 1},
        {{1, TokenID::PLUS, 2}, 2},
        {{2, TokenID::PLUS, 1}, 2},
        {{2, TokenID::PLUS, 2}, 2},
        // binary mins
        {{1, TokenID::MINUS, 1}, 1},
        {{1, TokenID::MINUS, 2}, 2},
        {{2, TokenID::MINUS, 1}, 2},
        {{2, TokenID::MINUS, 2}, 2},
        // unary minus
        {{0, TokenID::MINUS, 1}, 1},
        {{0, TokenID::MINUS, 2}, 2},
        // binary mult
        {{1, TokenID::STAR, 1}, 1},
        {{1, TokenID::STAR, 2}, 2},
        {{2, TokenID::STAR, 1}, 2},
        {{2, TokenID::STAR, 2}, 2},
        // binary div
        {{1, TokenID::SLASH, 1}, 1},
        {{1, TokenID::SLASH, 2}, 2},
        {{2, TokenID::SLASH, 1}, 2},
        {{2, TokenID::SLASH, 2}, 2},
        // binary greater than
        {{1, TokenID::GT, 1}, 3},
        {{1, TokenID::GT, 2}, 3},
        {{2, TokenID::GT, 1}, 3},
        {{2, TokenID::GT, 2}, 3},
        // unary less than
        {{1, TokenID::LT, 1}, 3},
        {{1, TokenID::LT, 2}, 3},
        {{2, TokenID::LT, 1}, 3},
        {{2, TokenID::LT, 2}, 3},
        // binary greater than or equal
        {{1, TokenID::GEQ, 1}, 3},
        {{1, TokenID::GEQ, 2}, 3},
        {{2, TokenID::GEQ, 1}, 3},
        {{2, TokenID::GEQ, 2}, 3},
        // binary less than or equal
        {{1, TokenID::LEQ, 1}, 3},
        {{1, TokenID::LEQ, 2}, 3},
        {{2, TokenID::LEQ, 1}, 3},
        {{2, TokenID::LEQ, 2}, 3},
        // binary eqality
        {{1, TokenID::EQ_EQ, 1}, 3},
        {{1, TokenID::EQ_EQ, 2}, 3},
        {{2, TokenID::EQ_EQ, 1}, 3},
        {{2, TokenID::EQ_EQ, 2}, 3},
        {{3, TokenID::EQ_EQ, 3}, 3},
        // binary not equality
        {{1, TokenID::BANG_EQ, 1}, 3},
        {{1, TokenID::BANG_EQ, 2}, 3},
        {{2, TokenID::BANG_EQ, 1}, 3},
        {{2, TokenID::BANG_EQ, 2}, 3},
        {{3, TokenID::BANG_EQ, 3}, 3},
        // unary negation
        {{0, TokenID::BANG, 3}, 3},
    };