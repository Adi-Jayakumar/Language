#pragma once
#include "token.h"
#include <vector>
#include <unordered_map>

typedef uint8_t TypeID;

struct VarID
{
    std::string name;
    TypeID type;
};

// map from literal TokenIDs to 
static const std::unordered_map<TokenID, TypeID> DefaultTypeMap{
    {TokenID::INT_L, 1},
    {TokenID::DOUBLE_L, 2},
    {TokenID::BOOL_L, 3}};

// map from string typenames to uint8_t type ids.
static std::unordered_map<std::string, TypeID> TypeNameMap{{"int", 1}, {"double", 2}, {"bool", 3}};

static std::vector<VarID> Locals;