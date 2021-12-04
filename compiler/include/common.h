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

const size_t NUM_DEF_TYPES = DefaultTypeMap.size() + 1;

bool operator==(const TypeData &left, const TypeData &right);
bool operator!=(const TypeData &left, const TypeData &right);

struct TypeDataHasher
{
    size_t operator()(const TypeData &t) const
    {
        std::hash<size_t> stHasher;
        std::hash<TypeID> tHasher;
        return stHasher(t.isArray) ^ tHasher(t.type);
    }
};