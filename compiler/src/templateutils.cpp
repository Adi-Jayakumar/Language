#include "templateutils.h"

std::unordered_map<TypeID, TypeData> GetTemplateMap()
{
    std::unordered_map<TypeID, TypeData> result;
    for (auto &type : AllTypes)
        result[type.type] = type;
    return result;
}

TypeSubstitution::TypeSubstitution()
{
    map = GetTemplateMap();
    loc = Token(TokenID::BEG, "", 0);
}

TypeSubstitution::TypeSubstitution(Token _loc)
{
    map = GetTemplateMap();
    loc = _loc;
}

TypeData TypeSubstitution::operator[](TypeData t)
{
    TypeData result = map[t.type];
    result.type += t.isArray;
    return result;
}