#include "templateutils.h"

std::unordered_map<TypeID, TypeData> GetTemplateMap()
{
    std::unordered_map<TypeID, TypeData> result;
    for (auto &type : AllTypes)
        result[type.type] = type;
    return result;
}
