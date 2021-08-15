#pragma once
#include "common.h"
#include "idstructs.h"

const std::vector<FuncID> NativeFunctions{
    FuncID(GetTypeNameMap()["void"], "Print", {VOID_ARRAY}, FunctionType::NATIVE),
    FuncID(GetTypeNameMap()["void"], "ToString", {GetTypeNameMap()["void"]}, FunctionType::NATIVE),
};