#include "symboltable.h"

std::vector<FuncID> GetNativeFunctions()
{
    std::vector<FuncID> natives;
    natives.push_back(FuncID({false, 0}, "Print", {{true, 0}}));
    return natives;
}

SymbolTable::SymbolTable()
{
    nativeFunctions = GetNativeFunctions();
    for (const auto &kv : NativeFunctions)
    {
        nativeFunctions.push_back(kv.first);
    }
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

size_t SymbolTable::FindVarByName(std::string &name)
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

    int offsetLoc = static_cast<int>(varIndex) - static_cast<int>(funcVarBegin);
    return offsetLoc < 0 ? varIndex : varIndex - funcVarBegin;
}

void SymbolTable::AddFunc(TypeData ret, std::string name, std::vector<TypeData> argtypes)
{
    funcs.push_back(FuncID(ret, name, argtypes));
}

size_t SymbolTable::FindFunc(std::string &name, std::vector<TypeData> &argtypes)
{
    for (size_t i = funcs.size() - 1; (int)i >= 0; i--)
    {
        if (argtypes.size() == funcs[i].argtypes.size() && name == funcs[i].name)
        {
            bool doesMatch = true;
            for (size_t j = 0; j < argtypes.size(); j++)
            {
                if (argtypes[j] != funcs[i].argtypes[j])
                {
                    doesMatch = false;
                    break;
                }
            }
            if (doesMatch)
                return i;
        }
    }
    return SIZE_MAX;
}

size_t SymbolTable::FindNativeFunctions(const std::vector<TypeData> &args)
{
    size_t index = SIZE_MAX;

    for (size_t i = 0; i < nativeFunctions.size(); i++)
    {
        if (MatchToNativeFuncs(nativeFunctions[i].argtypes, args))
        {
            index = i;
            break;
        }
    }

    return index;
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

void SymbolTable::AddStruct(StructID &s)
{
    strcts.push_back(s);
}

size_t SymbolTable::FindStruct(const TypeData &td)
{
    for (size_t i = strcts.size() - 1; (int)i >= 0; i--)
    {
        if (strcts[i].type.type == td.type)
            return i;
    }

    return SIZE_MAX;
}

bool MatchToNativeFuncs(const std::vector<TypeData> &native, const std::vector<TypeData> &args)
{
    if (native.size() == 0 && args.size() != 0)
        return false;

    TypeData matchMoreThanOne = {true, 0};
    if (native.size() == 1 && native[0] == matchMoreThanOne && args.size() > 0)
        return true;

    TypeData matchOne = {0, false};
    if (args.size() == 1 && native.size() == 1 && native[0] == matchOne)
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

void LibraryError(const std::string &msg)
{
    Error e("[LIBRARY ERROR]\n" + msg + "\n");
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

std::vector<std::string> SymbolTable::GetLibraryFunctionNames(const std::string &)
{
    // in reality: load library, call FunctionPrototypes/find the required constant
    return {"Sin                : double - double",
            "Cos                : double - double",
            "Tan                : double - double",
            "GetPi              :        - double",
            "EuclideanDist      : double, double - double"};
}

void SymbolTable::ParseLibraryFunction(std::string &func)
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

    std::cout << retType << " " << name << "(";
    for (const TypeData &arg : argtypes)
        std::cout << arg << ", ";

    std::cout << ")" << std::endl;

    funcs.push_back(FuncID(retType, name, argtypes));
}