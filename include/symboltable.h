#pragma once
#include "idstructs.h"

struct SymbolTable
{
    std::vector<VarID> vars;
    std::vector<FuncID> funcs;
    std::vector<StructID> strcts;

    size_t depth = 0;
    size_t funcVarBegin;

    void AddVar(TypeData, std::string);

    // void AddFunc(FuncID &);
    // void AddStruct(StructID &);

    bool IsVarInScope(std::string &);
    size_t FindVarByName(std::string &);
    // size_t FindFunc(std::string &, std::vector<TypeData> &);
    // size_t FindStruct(TypeData &);
};