#pragma once
#include "common.h"
#include "idstructs.h"

const std::vector<FuncID> NativeFunctions{
    FuncID(GetTypeNameMap()["void"], "Print", {{true, 0}}, 0),
    FuncID(GetTypeNameMap()["void"], "ToString", {GetTypeNameMap()["void"]}, 0),
};