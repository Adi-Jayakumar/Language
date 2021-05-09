#include "symboltable.h"

SymbolTable::SymbolTable()
{
    
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