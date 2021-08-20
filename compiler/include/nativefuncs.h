#pragma once
#include "common.h"
#include "idstructs.h"

const std::vector<FuncID> NativeFunctions{
    FuncID(GetTypeNameMap()["void"], "Print", std::vector<TypeData>(), {VOID_ARRAY}, FunctionType::NATIVE, 0),
    FuncID(GetTypeNameMap()["void"], "ToString", std::vector<TypeData>(), {VOID_TYPE}, FunctionType::NATIVE, 0),
};