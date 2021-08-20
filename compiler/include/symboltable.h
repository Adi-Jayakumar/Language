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
    std::vector<FuncID> clibFunctions;
    std::vector<StructID> strcts;

    size_t depth = 0;
    size_t funcVarBegin = 0;

    SymbolTable();

    bool CanAssign(const TypeData &, const TypeData &);

    void AddVar(TypeData, std::string);
    void AddFunc(FuncID);
    void AddCLibFunc(FuncID);
    void AddStruct(StructID);

    bool IsVarInScope(std::string &name);
    VarID *GetVar(std::string &name);
    FuncID *GetFunc(std::string &name, std::vector<TypeData> &argtypes);
    StructID *GetStruct(const TypeData &type);

    FuncID *FindNativeFunctions(const std::vector<TypeData> &args, const std::string &name);
    FuncID *FindCLibraryFunctions(const std::vector<TypeData> &args, const std::string &name);

    bool IsEqual(const std::vector<TypeData> &actual, const std::vector<TypeData> &given);
    bool CanAssignAll(const std::vector<TypeData> &actual, const std::vector<TypeData> &given);

    size_t GetVarStackLoc(std::string &name);

    void PopUntilSized(size_t size);
    void CleanUpCurDepth();

    //-------------------LIBRARY-------------------//
    std::vector<std::string> GetLibraryFunctionNames(const std::string &libname);
    FuncID ParseLibraryFunction(std::string &func);
};

void SymbolError(const std::string &msg);
void LibraryError(const std::string &msg);
std::vector<std::string> SplitStringByChar(std::string &, char);
std::string TrimFrontBack(std::string &);

bool MatchToNativeFuncs(const std::vector<TypeData> &, const std::vector<TypeData> &);