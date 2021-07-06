#pragma once
#include "idstructs.h"

static const std::unordered_map<FuncID, size_t, FuncIDHasher, FuncIDEq> NativeFunctions{
    {{{0, false}, "Print", {{0, true}}, false}, 0},
    {{{0, false}, "ToString", {{0, false}}, false}, 1},
};