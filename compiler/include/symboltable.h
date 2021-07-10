#pragma once
#include "idstructs.h"
#include "nativefuncs.h"
#include "perror.h"
#include <dlfcn.h>
#include <sstream>

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

    void AddFunc(TypeData, std::string, std::vector<TypeData>, bool);
    void AddFunc(FuncID &);
    void AddStruct(StructID &);

    bool IsVarInScope(std::string &);
    size_t FindVarByName(std::string &);
    size_t FindFunc(std::string &, std::vector<TypeData> &);
    size_t FindNativeFunctions(const std::vector<TypeData> &, const std::string &);

    void PopUntilSized(size_t size);
    void CleanUpCurDepth();

    size_t FindStruct(const TypeData &);

    //-------------------LIBRARY-------------------//
    std::vector<std::string> GetLibraryFunctionNames(const std::string &libname);
    FuncID ParseLibraryFunction(std::string &func);
};

void SymbolError(const std::string &msg);
void LibraryError(const std::string &msg);
std::vector<std::string> SplitStringByChar(std::string &, char);
std::string TrimFrontBack(std::string &);

bool MatchToNativeFuncs(const std::vector<TypeData> &, const std::vector<TypeData> &);