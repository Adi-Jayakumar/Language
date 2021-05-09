#pragma once
#include "idstructs.h"
#include "nativefuncs.h"

struct SymbolTable
{
    std::vector<VarID> vars;
    std::vector<FuncID> funcs;
    std::vector<FuncID> nativeFunctions;
    std::vector<StructID> strcts;

    size_t depth = 0;
    size_t funcVarBegin = 0;

    SymbolTable();

    void AddVar(TypeData, std::string);

    void AddFunc(TypeData, std::string, std::vector<TypeData>);
    void AddStruct(StructID &);

    bool IsVarInScope(std::string &);
    size_t FindVarByName(std::string &);
    size_t FindFunc(std::string &, std::vector<TypeData> &);
    size_t FindNativeFunctions(const std::vector<TypeData>&);

    void PopUntilSized(size_t size);

    size_t FindStruct(const TypeData &);
};

bool MatchToNativeFuncs(const std::vector<TypeData> &, const std::vector<TypeData> &);