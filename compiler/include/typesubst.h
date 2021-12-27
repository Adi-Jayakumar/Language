#pragma once
#include "token.h"
#include "typedata.h"
#include <unordered_map>

class TypeSubstituter
{
public:
    std::unordered_map<TypeData, TypeData> subst;
    Token loc;
    TypeSubstituter()
    {
        for (const auto &def : AllTypes)
            subst[def] = def;
    }

    TypeSubstituter(const std::vector<TypeData> &templates,
                    const std::vector<TypeData> &new_types,
                    const Token &_loc) : loc(_loc)
    {
        for (const auto &def : AllTypes)
            subst[def] = def;

        // StaticAnalyser should ensure that their sizes are same
        for (size_t i = 0; i < templates.size(); i++)
            subst[templates[i]] = new_types[i];
    };

    TypeData operator[](TypeData &tmp_type);
    void PrintTypeMappings(std::ostream &out);
};
