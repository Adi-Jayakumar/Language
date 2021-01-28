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
        return (l << 16) | (r << 8) | (tok);
    }
};

// map of the name of a native function and its return type
static const std::unordered_map<std::string, TypeID> NativeFunctions{
    {"Print", 0},
};

static const std::unordered_map<TypeInfo, TypeID, TypeInfoHasher>
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
        {{3, TokenID::EQ_EQ, 3}, 3},
        {{1, TokenID::BANG_EQ, 1}, 3},
        {{1, TokenID::BANG_EQ, 2}, 3},
        {{2, TokenID::BANG_EQ, 1}, 3},
        {{2, TokenID::BANG_EQ, 2}, 3},
        {{3, TokenID::BANG_EQ, 3}, 3},
        {{0, TokenID::BANG, 3}, 3},
    };