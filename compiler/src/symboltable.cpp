#include "symboltable.h"

SymbolTable::SymbolTable()
{
    nativeFunctions = NativeFunctions;
}

bool SymbolTable::CanAssign(const TypeData &varType, const TypeData &valType)
{
    if (varType.isArray != valType.isArray)
        return false;

    if (varType.type == 6 || valType.type == 6)
        return true;
    else if (varType.type == 1 && valType.type == 2)
        return true;
    else if (varType.type == 2 && valType.type == 1)
        return true;

    if (varType.type > 6 && valType.type > 6)
    {
        if (varType == valType)
            return true;

        StructID *sVal = GetStruct(valType);
        TypeData parent = sVal->parent;

        if (parent == VOID_TYPE)
            return varType == valType;

        parent.isArray = valType.isArray;
        return CanAssign(varType, parent);
    }

    return varType == valType;
}

void SymbolTable::AddVar(TypeData type, std::string name)
{
    vars.push_back(VarID(type, name, depth, vars.size()));
}

bool SymbolTable::IsVarInScope(std::string &name)
{
    size_t varIndex = SIZE_MAX;

    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name && vars[i].depth == depth)
        {
            varIndex = i;
            break;
        }
    }

    return varIndex != SIZE_MAX;
}

VarID *SymbolTable::GetVar(std::string &name)
{
    size_t varIndex = SIZE_MAX;

    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name)
        {
            varIndex = i;
            break;
        }
    }

    if (varIndex == SIZE_MAX)
        return nullptr;

    return &vars[varIndex];
}

size_t SymbolTable::GetVarStackLoc(std::string &name)
{
    size_t varIndex = SIZE_MAX;

    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name)
        {
            varIndex = i;
            break;
        }
    }

    if (funcVarBegin == 0)
        return varIndex;
    else
        return varIndex - funcVarBegin;
}

void SymbolTable::AddFunc(FuncID func)
{
    funcs.emplace_back(func);
}

void SymbolTable::AddCLibFunc(FuncID func)
{
    clibFunctions.push_back(func);
}

FuncID *SymbolTable::GetFunc(std::string &name, std::vector<TypeData> &args)
{
    for (auto &f : funcs)
    {
        if (f.name == name && IsEqual(f.argtypes, args))
            return &f;
    }

    for (auto &f : funcs)
    {
        if (f.name == name && CanAssignAll(f.argtypes, args))
            return &f;
    }

    FuncID *f = FindCLibraryFunctions(args, name);
    if (f != nullptr)
        return f;

    f = FindNativeFunctions(args, name);
    return f;
}

FuncID *SymbolTable::FindNativeFunctions(const std::vector<TypeData> &args, const std::string &name)
{
    std::cout << "name " << name << std::endl;
    for (auto &nf : nativeFunctions)
    {
        if (nf.name == name && MatchToNativeFuncs(nf.argtypes, args))
            return &nf;
    }
    return nullptr;
}

bool MatchToNativeFuncs(const std::vector<TypeData> &native, const std::vector<TypeData> &args)
{
    if (native.size() == 0 && args.size() != 0)
        return false;

    TypeData matchMoreThanOne = VOID_ARRAY;
    if (native.size() == 1 && native[0] == matchMoreThanOne && args.size() > 0)
    {
        for (const auto &arg : args)
        {
            if (arg == VOID_TYPE)
                SymbolError("Cannot pass object of type 'void' as a function argument");
        }
        return true;
    }

    if (args.size() == 1 && native.size() == 1 && native[0] == VOID_TYPE)
        return true;

    if (native.size() != args.size())
        return false;

    bool areSame = true;
    for (size_t i = 0; i < native.size(); i++)
    {
        if (native[i] != args[i])
        {
            areSame = false;
            break;
        }
    }

    return areSame;
}

FuncID *SymbolTable::FindCLibraryFunctions(const std::vector<TypeData> &args, const std::string &name)
{
    for (auto &lf : clibFunctions)
    {
        if (lf.name == name && IsEqual(lf.argtypes, args))
            return &lf;
    }

    for (auto &lf : clibFunctions)
    {
        if (lf.name == name && CanAssignAll(lf.argtypes, args))
            return &lf;
    }
    return nullptr;
}

bool SymbolTable::IsEqual(const std::vector<TypeData> &actual, const std::vector<TypeData> &given)
{
    if (actual.size() != given.size())
        return false;

    bool equal = true;
    for (size_t i = 0; i < actual.size(); i++)
    {
        if (actual[i] != given[i])
        {
            equal = false;
            break;
        }
    }
    return equal;
}

bool SymbolTable::CanAssignAll(const std::vector<TypeData> &actual, const std::vector<TypeData> &given)
{
    if (actual.size() != given.size())
        return false;

    bool equal = true;
    for (size_t i = 0; i < actual.size(); i++)
    {
        if (!CanAssign(actual[i], given[i]))
        {
            equal = false;
            break;
        }
    }
    return equal;
}

void SymbolTable::PopUntilSized(size_t size)
{
    if (vars.size() <= size)
        return;

    while (vars.size() > size)
        vars.pop_back();
}

void SymbolTable::CleanUpCurDepth()
{
    while (vars.size() > 0 && vars.back().depth == depth)
        vars.pop_back();

    if (vars.size() == 1 && vars[0].depth == depth)
        vars.clear();
}

void SymbolTable::AddStruct(StructID s)
{
    strcts.push_back(s);
}

StructID *SymbolTable::GetStruct(const TypeData &td)
{
    for (size_t i = strcts.size() - 1; (int)i >= 0; i--)
    {
        if (strcts[i].type.type == td.type)
            return &strcts[i];
    }

    return nullptr;
}

void SymbolError(const std::string &msg)
{
    Error e("[Symbol ERROR]\n" + msg + "\n");
    throw e;
}

void LibraryError(const std::string &msg)
{
    Error e("[LIBRARY ERROR]\n" + msg + "\n");
    throw e;
}

std::vector<std::string> SplitStringByChar(std::string &s, char c)
{
    std::stringstream stream(s);
    std::string segment;
    std::vector<std::string> split;

    while (std::getline(stream, segment, c))
        split.push_back(segment);

    return split;
}

std::string TrimFrontBack(std::string &str)
{
    size_t first = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        first = i;
        if (!isspace(str[i]))
            break;
    }

    size_t last = str.length() - 1;
    for (size_t j = str.length() - 1; (int)j >= 0; j--)
    {
        last = j;
        if (!isspace(str[j]))
            break;
    }

    return str.substr(first, last - first + 1);
}

#define DL_ERROR(errorMsg)                  \
    errorMsg = dlerror();                   \
    if (errorMsg != NULL)                   \
    {                                       \
        std::cerr << errorMsg << std::endl; \
    }

std::vector<std::string> SymbolTable::GetLibraryFunctionNames(const std::string &modulename)
{
    std::string libpath = "./lib/lib" + modulename + ".so";
    char *errorMsg;

    void *handle = dlopen(libpath.c_str(), RTLD_LAZY);
    DL_ERROR(errorMsg)

    const char *const *LibraryFunctions;
    *(void **)&LibraryFunctions = dlsym(handle, "LibraryFunctions");
    DL_ERROR(errorMsg)

    size_t *numfuncs;
    *(void **)&numfuncs = dlsym(handle, "NumLibFunctions");
    DL_ERROR(errorMsg)

    std::vector<std::string> libfuncs;
    for (size_t i = 0; i < *numfuncs; i++)
        libfuncs.push_back(LibraryFunctions[i]);

    dlclose(handle);
    return libfuncs;
}

FuncID SymbolTable::ParseLibraryFunction(std::string &func)
{
    std::vector<std::string> name_prototype = SplitStringByChar(func, ':');

    if (name_prototype.size() != 2)
        LibraryError("Invalid function prototype definition for function '" + func + "'");

    std::string name = TrimFrontBack(name_prototype[0]);
    std::vector<std::string> input_ret = SplitStringByChar(name_prototype[1], '-');

    if (input_ret.size() > 2 || input_ret.size() == 1)
        LibraryError("Invalid separation between argument and return types in function '" + func + "'");

    std::vector<std::string> args = SplitStringByChar(input_ret[0], ',');

    for (size_t i = 0; i < args.size(); i++)
        args[i] = TrimFrontBack(args[i]);

    std::vector<TypeData> argtypes;
    if (args.size() != 1 || args[0] != " ")
    {
        for (size_t i = 0; i < args.size(); i++)
        {
            if (GetTypeNameMap().find(args[i]) == GetTypeNameMap().end())
                LibraryError("Invalid argumnent type '" + args[i] + "'");
            argtypes.push_back(GetTypeNameMap()[args[i]]);
        }
    }

    std::string ret = TrimFrontBack(input_ret[1]);
    if (GetTypeNameMap().find(ret) == GetTypeNameMap().end())
        LibraryError("Invalid return type '" + ret + "'");
    TypeData retType = GetTypeNameMap()[ret];

    return FuncID(retType, name, argtypes, FunctionType::LIBRARY);
}