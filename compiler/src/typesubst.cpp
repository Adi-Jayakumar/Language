#include "typesubst.h"

void TypeSubstituter::PrintTypeMappings(std::ostream &out)
{
    for (auto &kv : subst)
        out << kv.first << " --> " << kv.second << std::endl;
}

void TypeSubstituter::AddType(const TypeData &type)
{
    subst[type] = type;
}

TypeData TypeSubstituter::operator[](TypeData &tmp_type)
{
    TypeData base_type(0, tmp_type.type);
    TypeData new_type = subst[base_type];
    new_type.is_array += tmp_type.is_array;

    for (size_t i = 0; i < tmp_type.tmps.size(); i++)
        new_type.tmps.push_back((*this)[tmp_type.tmps[i]]);

    return new_type;
}
