#include "symboltable.h"

SymbolTable::SymbolTable()
{
    bpOffset = 0;
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

size_t SymbolTable::SizeOf(const TypeData &type)
{
    if (type.isArray)
        return ARRAY_SIZE;
    else if (type == INT_TYPE)
        return INT_SIZE;
    else if (type == DOUBLE_TYPE)
        return DOUBLE_SIZE;
    else if (type == BOOL_TYPE)
        return BOOL_SIZE;
    else if (type == STRING_TYPE)
        return STRING_SIZE;
    else if (type == CHAR_TYPE)
        return CHAR_SIZE;
    else if (type == NULL_TYPE)
        return NULL_SIZE;

    StructID *sid = GetStruct(type);
    size_t res = 0;

    for (const auto &member : sid->memTypes)
        res += SizeOf(member);

    return res;
}

size_t SymbolTable::GetCurOffset()
{
    return bpOffset;
}

size_t SymbolTable::GetNewVarOffset(const TypeData &type)
{
    return bpOffset + (vars.size() ? SizeOf(vars.back().type) : 0);
}

void SymbolTable::AddVar(const TypeData &type, const std::string &name, const size_t size)
{
    vars.push_back(VarID(type, name, depth, size));
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

size_t SymbolTable::GetVariableStackLoc(std::string &name)
{
    size_t loc = 0;
    for (const auto &var : vars)
    {
        if (name == var.name)
            return loc;
        loc += var.size;
    }

    return SIZE_MAX;
}

void SymbolTable::AddFunc(const FuncID &func)
{
    funcs.emplace_back(func);
}

void SymbolTable::AddCLibFunc(const FuncID &func)
{
    clibFunctions.push_back(func);
}

FuncID *SymbolTable::GetFunc(std::string &name, std::vector<TypeData> &templates, std::vector<TypeData> &args)
{
    for (auto &f : funcs)
    {
        if (f.name == name && IsEqual(f.argtypes, args))
            return &f;
        if (f.name == name && MatchTemplateFunction(templates, args, f.templates, f.argtypes))
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

size_t SymbolTable::GetUDFuncNum(FuncID *fid)
{
    for (size_t i = 0; i < funcs.size(); i++)
    {
        if (fid == &funcs[i])
            return i;
    }
    return SIZE_MAX;
}

size_t SymbolTable::GetCLibFuncNum(FuncID *fid)
{
    for (size_t i = 0; i < clibFunctions.size(); i++)
    {
        if (fid == &clibFunctions[i])
            return i;
    }
    return SIZE_MAX;
}

size_t SymbolTable::GetNativeFuncNum(FuncID *fid)
{
    for (size_t i = 0; i < nativeFunctions.size(); i++)
    {
        if (fid == &nativeFunctions[i])
            return i;
    }
    return SIZE_MAX;
}

bool SymbolTable::MatchTemplateFunction(std::vector<TypeData> &templates, std::vector<TypeData> &args,
                                        std::vector<TypeData> f_templates, std::vector<TypeData> f_args)
{
    if (templates.size() != f_templates.size() || args.size() != f_args.size())
        return false;

    std::unordered_map<TypeID, TypeData> templateMap = GetTemplateMap();
    for (size_t i = 0; i < templates.size(); i++)
        templateMap[f_templates[i].type] = templates[i];

    for (size_t j = 0; j < f_args.size(); j++)
    {
        TypeData replacement = templateMap[f_args[j].type];
        f_args[j].type = replacement.type;
        f_args[j].isArray += replacement.isArray;
    }

    return IsEqual(f_args, args) || CanAssignAll(f_args, args);
}

FuncID *SymbolTable::FindNativeFunctions(const std::vector<TypeData> &args, const std::string &name)
{
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

void SymbolTable::AddStruct(const StructID &s)
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
        exit(3);                            \
    }

std::vector<std::string> SymbolTable::GetLibraryFunctionNames(const std::string &modulename)
{
    std::string libpath = "../runtime/lib/lib" + modulename + ".so";
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

    return FuncID(retType, name, std::vector<TypeData>(), argtypes, FunctionType::LIBRARY, 0);
}