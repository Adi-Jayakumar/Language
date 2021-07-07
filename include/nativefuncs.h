#pragma once
#include "idstructs.h"

static const std::unordered_map<FuncID, size_t, FuncIDHasher, FuncIDEq> NativeFunctions{

    {FuncID({0, false}, "Print", {{true, 0}}, 0), 0},
    {FuncID({0, false}, "ToString", {{false, 0}}, 0), 1},

};