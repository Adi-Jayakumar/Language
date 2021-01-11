#pragma once
#include "token.h"
#include <unordered_map>


static const std::unordered_map<TokenID, uint8_t> DefaultTypeMap{
    {TokenID::INT_L, 1},
    {TokenID::DOUBLE_L, 2},
    {TokenID::BOOL_L, 3}};

// map from string typenames to uint8_t type ids.
static std::unordered_map<std::string, uint8_t> TypeNameMap{{"int", 1}, {"double", 2}, {"bool", 3}};
