#pragma once
#include <string>
#include <vector>

// const std::vector<FuncID> NativeFunctions{
//     FuncID(GetTypeNameMap()["void"], "Print", std::vector<TypeData>(), {VOID_ARRAY}, FunctionType::NATIVE, 0),
//     FuncID(GetTypeNameMap()["void"], "ToString", std::vector<TypeData>(), {VOID_TYPE}, FunctionType::NATIVE, 0),
// };

const std::vector<std::string> NativeFunctions{
    "void Print(int)",
    "void Print(double)",
    "void Print(bool)",
    "void Print(string)",
    "void Print(char)",
    "int Length(string)"};