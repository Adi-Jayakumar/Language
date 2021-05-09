#pragma once
#include "idstructs.h"

std::unordered_map<FuncID, size_t> NativeFunctions{
    {{{0, false}, "Print", {{0, true}}}, 0},
    {{{0, false}, "ToString", {{0, false}}}, 1},
    };