#include "symboltable.h"

SymbolTable::SymbolTable()
{
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

    TypeData matchOne = {false, 0};
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