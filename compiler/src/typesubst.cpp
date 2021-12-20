#include "typesubst.h"

void TypeSubstituter::PrintTypeMappings()
{
    for (auto &kv : subst)
        std::cout << kv.first << " --> " << kv.second << std::endl;
}

TypeData TypeSubstituter::operator[](TypeData &tmp_type)
{
    TypeData base_type(tmp_type.is_array, tmp_type.type);
    TypeData new_type = subst[base_type];
    new_type.is_array += tmp_type.is_array;
    return new_type;
}
