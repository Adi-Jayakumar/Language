#pragma once
#include "token.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef uint8_t TypeID;

// map from literal TokenIDs to
static const std::unordered_map<TokenID, TypeID> DefaultTypeMap{
    {TokenID::INT_L, 1},
    {TokenID::DOUBLE_L, 2},
    {TokenID::BOOL_L, 3},
    {TokenID::STRING_L, 4},
    {TokenID::CHAR_L, 5}};

struct TypeData
{
    bool isArray = false;
    TypeID type = 0;
};

std::string ToString(const TypeData &td);
std::ostream &operator<<(std::ostream &out, const TypeData &td);
bool operator==(const TypeData &left, const TypeData &right);
bool operator!=(const TypeData &left, const TypeData &right);

struct TypeInfo
{
    TypeData left;
    TokenID t;
    TypeData right;
};

bool operator==(const TypeInfo &l, const TypeInfo &r);

// map from string typenames to uint8_t type ids.
static std::unordered_map<std::string, TypeData> TypeNameMap{{"void", {false, 0}}, {"int", {false, 1}}, {"double", {false, 2}}, {"bool", {false, 3}}, {"string", {false, 4}}, {"char", {false, 5}}};
static std::unordered_map<TypeID, std::string> TypeStringMap{{0, "void"}, {1, "int"}, {2, "double"}, {3, "bool"}, {4, "string"}, {5, "char"}};

static std::unordered_set<std::string> NativeFunctions{"Print", "ToString"};

struct TypeInfoHasher
{
    size_t operator()(const TypeInfo &t) const
    {
        std::hash<bool> bHasher;
        std::hash<TypeID> tHasher;
        std::hash<uint8_t> u8Hasher;
        size_t l = bHasher(t.left.isArray) ^ tHasher(t.left.type);
        size_t r = bHasher(t.right.isArray) ^ tHasher(t.right.type);
        size_t op = u8Hasher(static_cast<uint8_t>(t.t));
        return l ^ r ^ op;
    }
};

static const std::unordered_map<TypeInfo, TypeData, TypeInfoHasher>
    OperatorMap{
        // binary plus
        {{{false, 1}, TokenID::PLUS, {false, 1}}, {false, 1}},
        {{{false, 1}, TokenID::PLUS, {false, 2}}, {false, 2}},
        {{{false, 2}, TokenID::PLUS, {false, 1}}, {false, 2}},
        {{{false, 2}, TokenID::PLUS, {false, 2}}, {false, 2}},
        // binary mins
        {{{false, 1}, TokenID::MINUS, {false, 1}}, {false, 1}},
        {{{false, 1}, TokenID::MINUS, {false, 2}}, {false, 2}},
        {{{false, 2}, TokenID::MINUS, {false, 1}}, {false, 2}},
        {{{false, 2}, TokenID::MINUS, {false, 2}}, {false, 2}},
        // unary minus
        {{{false, 0}, TokenID::MINUS, {false, 1}}, {false, 1}},
        {{{false, 0}, TokenID::MINUS, {false, 2}}, {false, 2}},
        // binary mult
        {{{false, 1}, TokenID::STAR, {false, 1}}, {false, 1}},
        {{{false, 1}, TokenID::STAR, {false, 2}}, {false, 2}},
        {{{false, 2}, TokenID::STAR, {false, 1}}, {false, 2}},
        {{{false, 2}, TokenID::STAR, {false, 2}}, {false, 2}},
        // binary div
        {{{false, 1}, TokenID::SLASH, {false, 1}}, {false, 1}},
        {{{false, 1}, TokenID::SLASH, {false, 2}}, {false, 2}},
        {{{false, 2}, TokenID::SLASH, {false, 1}}, {false, 2}},
        {{{false, 2}, TokenID::SLASH, {false, 2}}, {false, 2}},
        // binary greater than
        {{{false, 1}, TokenID::GT, {false, 1}}, {false, 3}},
        {{{false, 1}, TokenID::GT, {false, 2}}, {false, 3}},
        {{{false, 2}, TokenID::GT, {false, 1}}, {false, 3}},
        {{{false, 2}, TokenID::GT, {false, 2}}, {false, 3}},
        // unary less than
        {{{false, 1}, TokenID::LT, {false, 1}}, {false, 3}},
        {{{false, 1}, TokenID::LT, {false, 2}}, {false, 3}},
        {{{false, 2}, TokenID::LT, {false, 1}}, {false, 3}},
        {{{false, 2}, TokenID::LT, {false, 2}}, {false, 3}},
        // binary greater than or equal
        {{{false, 1}, TokenID::GEQ, {false, 1}}, {false, 3}},
        {{{false, 1}, TokenID::GEQ, {false, 2}}, {false, 3}},
        {{{false, 2}, TokenID::GEQ, {false, 1}}, {false, 3}},
        {{{false, 2}, TokenID::GEQ, {false, 2}}, {false, 3}},
        // binary less than or equal
        {{{false, 1}, TokenID::LEQ, {false, 1}}, {false, 3}},
        {{{false, 1}, TokenID::LEQ, {false, 2}}, {false, 3}},
        {{{false, 2}, TokenID::LEQ, {false, 1}}, {false, 3}},
        {{{false, 2}, TokenID::LEQ, {false, 2}}, {false, 3}},
        // binary eqality
        {{{false, 1}, TokenID::EQ_EQ, {false, 1}}, {false, 3}},
        {{{false, 1}, TokenID::EQ_EQ, {false, 2}}, {false, 3}},
        {{{false, 2}, TokenID::EQ_EQ, {false, 1}}, {false, 3}},
        {{{false, 2}, TokenID::EQ_EQ, {false, 2}}, {false, 3}},
        {{{false, 3}, TokenID::EQ_EQ, {false, 3}}, {false, 3}},
        // binary not equality
        {{{false, 1}, TokenID::BANG_EQ, {false, 1}}, {false, 3}},
        {{{false, 1}, TokenID::BANG_EQ, {false, 2}}, {false, 3}},
        {{{false, 2}, TokenID::BANG_EQ, {false, 1}}, {false, 3}},
        {{{false, 2}, TokenID::BANG_EQ, {false, 2}}, {false, 3}},
        {{{false, 3}, TokenID::BANG_EQ, {false, 3}}, {false, 3}},
        // unary negation
        {{{false, 0}, TokenID::BANG, {false, 3}}, {false, 3}},
        // string concatenation
        {{{false, 4}, TokenID::PLUS, {false, 4}}, {false, 4}},
    };