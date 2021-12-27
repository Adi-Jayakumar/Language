#include "typesubst.h"

void TypeSubstituter::PrintTypeMappings(std::ostream &out)
{
    for (auto &kv : subst)
        out << kv.first << " --> " << kv.second << std::endl;
}

TypeData TypeSubstituter::operator[](TypeData &tmp_type)
{
    TypeData base_type(0, tmp_type.type);
    TypeData new_type = subst[base_type];
    new_type.is_array += tmp_type.is_array;
    return new_type;
}
