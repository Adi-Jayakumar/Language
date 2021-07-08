#pragma once
#include "idstructs.h"

const std::vector<FuncID> NativeFunctions{
    FuncID({0, false}, "Print", {{true, 0}}, 0),
    FuncID({0, false}, "ToString", {{false, 0}}, 0),
};