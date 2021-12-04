#pragma once
#include "token.h"
#include "typedata.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool operator==(const TypeData &left, const TypeData &right);
bool operator!=(const TypeData &left, const TypeData &right);

struct TypeDataHasher
{
    size_t operator()(const TypeData &t) const
    {
        std::hash<size_t> stHasher;
        std::hash<TypeID> tHasher;
        return stHasher(t.is_array) ^ tHasher(t.type);
    }
};