#pragma once
#include "idstructs.h"
#include "nativefuncs.h"
#include "perror.h"
#include "templateutils.h"
#include <dlfcn.h>
#include <sstream>

class SymbolTable
{
public:
    std::vector<VarID> vars;

private:
    std::vector<FuncID> funcs;
    std::vector<FuncID> nativeFunctions;
    std::vector<FuncID> clibFunctions;
    std::vector<StructID> strcts;

    size_t funcVarBegin = 0;
    size_t bpOffset;

public:
    size_t depth = 0;
    SymbolTable();

    bool CanAssign(const TypeData &target, const TypeData &given);

    // accepts any type apart from VOID_TYPE
    size_t SizeOf(const TypeData &type);

    // returns the offset from the base pointer of the last variable added to the stack
    size_t GetCurOffset();
    void UpdateBP(const size_t &offset) { bpOffset += offset; };

    void AddVar(const TypeData &type, const std::string &name, const bool &updateBP = false);
    void AddFunc(const FuncID &fid);
    void AddCLibFunc(const FuncID &fid);
    void AddStruct(const StructID &sid);

    bool IsVarInScope(std::string &name);
    // getters of identifiers
    VarID *GetVar(std::string &name);
    // returns SIZE_MAX if variable is not found
    size_t GetVariableStackLoc(std::string &name);

    FuncID *GetFunc(std::string &name, std::vector<TypeData> &templates, std::vector<TypeData> &argtypes);
    size_t GetUDFuncNum(FuncID *fid);
    size_t GetCLibFuncNum(FuncID *fid);
    size_t NumCFuncs() { return clibFunctions.size(); };
    size_t GetNativeFuncNum(FuncID *fid);

    StructID *GetStruct(const TypeData &type);

    bool MatchTemplateFunction(std::vector<TypeData> &templates, std::vector<TypeData> &args,
                               std::vector<TypeData> f_templates, std::vector<TypeData> f_args);
    FuncID *FindNativeFunctions(const std::vector<TypeData> &args, const std::string &name);
    FuncID *FindCLibraryFunctions(const std::vector<TypeData> &args, const std::string &name);

    bool IsEqual(const std::vector<TypeData> &actual, const std::vector<TypeData> &given);
    bool CanAssignAll(const std::vector<TypeData> &actual, const std::vector<TypeData> &given);

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