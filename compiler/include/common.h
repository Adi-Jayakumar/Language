#pragma once
#include "token.h"
#include "typedata.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

// map from literal TokenIDs to
static const std::unordered_map<TokenID, TypeID> DefaultTypeMap{
    {TokenID::INT_L, 1},
    {TokenID::DOUBLE_L, 2},
    {TokenID::BOOL_L, 3},
    {TokenID::STRING_L, 4},
    {TokenID::CHAR_L, 5},
    {TokenID::NULL_T, 6}};

bool operator==(const TypeData &left, const TypeData &right);
bool operator!=(const TypeData &left, const TypeData &right);
std::string ToString(const TypeData &td);
std::ostream &operator<<(std::ostream &out, const TypeData &td);

struct TypeDataHasher
{
    size_t operator()(const TypeData &t) const
    {
        std::hash<size_t> stHasher;
        std::hash<TypeID> tHasher;
        return stHasher(t.isArray) ^ tHasher(t.type);
    }
};

std::string ToString(const TypeData &td);
std::ostream &operator<<(std::ostream &out, const TypeData &td);

class TypeInfo
{
public:
    TypeData left;
    TokenID t;
    TypeData right;

    TypeInfo() = default;
    TypeInfo(TypeData _left, TokenID _t, TypeData _right) : left(_left), t(_t), right(_right){};
};

bool operator==(const TypeInfo &l, const TypeInfo &r);

// map from string typenames to uint8_t type ids.
std::unordered_map<std::string, TypeData> &GetTypeNameMap();
std::unordered_map<TypeID, std::string> &GetTypeStringMap();

struct TypeInfoHasher
{
    size_t operator()(const TypeInfo &t) const
    {
        std::hash<size_t> stHasher;
        std::hash<TypeID> tHasher;
        std::hash<uint8_t> u8Hasher;
        size_t l = stHasher(t.left.isArray) ^ tHasher(t.left.type);
        size_t r = stHasher(t.right.isArray) ^ tHasher(t.right.type);
        size_t op = u8Hasher(static_cast<uint8_t>(t.t));
        return l ^ r ^ op;
    }
};

bool CheckBinaryOperatorUse(const TypeInfo &);
TypeData GetBinaryOperatorType(const TypeInfo &);
bool CheckUnaryOperatorUse(const TypeInfo &);

static const std::unordered_map<TypeInfo, TypeData, TypeInfoHasher>
    OperatorMap{
        // binary plus
        {{INT_TYPE, TokenID::PLUS, INT_TYPE}, INT_TYPE},
        {{INT_TYPE, TokenID::PLUS, DOUBLE_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::PLUS, INT_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::PLUS, DOUBLE_TYPE}, DOUBLE_TYPE},
        // binary mins
        {{INT_TYPE, TokenID::MINUS, INT_TYPE}, INT_TYPE},
        {{INT_TYPE, TokenID::MINUS, DOUBLE_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::MINUS, INT_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::MINUS, DOUBLE_TYPE}, DOUBLE_TYPE},
        // unary minus
        {{VOID_TYPE, TokenID::MINUS, INT_TYPE}, INT_TYPE},
        {{VOID_TYPE, TokenID::MINUS, DOUBLE_TYPE}, DOUBLE_TYPE},
        // binary mult
        {{INT_TYPE, TokenID::STAR, INT_TYPE}, INT_TYPE},
        {{INT_TYPE, TokenID::STAR, DOUBLE_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::STAR, INT_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::STAR, DOUBLE_TYPE}, DOUBLE_TYPE},
        // binary div
        {{INT_TYPE, TokenID::SLASH, INT_TYPE}, INT_TYPE},
        {{INT_TYPE, TokenID::SLASH, DOUBLE_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::SLASH, INT_TYPE}, DOUBLE_TYPE},
        {{DOUBLE_TYPE, TokenID::SLASH, DOUBLE_TYPE}, DOUBLE_TYPE},
        // binary greater than
        {{INT_TYPE, TokenID::GT, INT_TYPE}, BOOL_TYPE},
        {{INT_TYPE, TokenID::GT, DOUBLE_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::GT, INT_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::GT, DOUBLE_TYPE}, BOOL_TYPE},
        // unary less than
        {{INT_TYPE, TokenID::LT, INT_TYPE}, BOOL_TYPE},
        {{INT_TYPE, TokenID::LT, DOUBLE_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::LT, INT_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::LT, DOUBLE_TYPE}, BOOL_TYPE},
        // binary greater than or equal
        {{INT_TYPE, TokenID::GEQ, INT_TYPE}, BOOL_TYPE},
        {{INT_TYPE, TokenID::GEQ, DOUBLE_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::GEQ, INT_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::GEQ, DOUBLE_TYPE}, BOOL_TYPE},
        // binary less than or equal
        {{INT_TYPE, TokenID::LEQ, INT_TYPE}, BOOL_TYPE},
        {{INT_TYPE, TokenID::LEQ, DOUBLE_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::LEQ, INT_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::LEQ, DOUBLE_TYPE}, BOOL_TYPE},
        // binary eqality
        {{INT_TYPE, TokenID::EQ_EQ, INT_TYPE}, BOOL_TYPE},
        {{INT_TYPE, TokenID::EQ_EQ, DOUBLE_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::EQ_EQ, INT_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::EQ_EQ, DOUBLE_TYPE}, BOOL_TYPE},
        {{BOOL_TYPE, TokenID::EQ_EQ, BOOL_TYPE}, BOOL_TYPE},
        // binary not equality
        {{INT_TYPE, TokenID::BANG_EQ, INT_TYPE}, BOOL_TYPE},
        {{INT_TYPE, TokenID::BANG_EQ, DOUBLE_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::BANG_EQ, INT_TYPE}, BOOL_TYPE},
        {{DOUBLE_TYPE, TokenID::BANG_EQ, DOUBLE_TYPE}, BOOL_TYPE},
        {{BOOL_TYPE, TokenID::BANG_EQ, BOOL_TYPE}, BOOL_TYPE},
        // unary negation
        {{VOID_TYPE, TokenID::BANG, BOOL_TYPE}, BOOL_TYPE},
        // string concatenation
        {{STRING_TYPE, TokenID::PLUS, STRING_TYPE}, STRING_TYPE},
    };