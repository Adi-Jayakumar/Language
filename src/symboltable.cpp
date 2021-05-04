#include "symboltable.h"

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

    return varIndex - funcVarBegin;
}