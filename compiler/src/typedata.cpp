#include "typedata.h"

bool operator==(const TypeData &left, const TypeData &right)
{
    return (left.type == right.type) && (left.is_array == right.is_array);
}

bool operator!=(const TypeData &left, const TypeData &right)
{
    return (left.type != right.type) || (left.is_array != right.is_array);
}

std::ostream &operator<<(std::ostream &out, const TypeData td)
{
    out << '(' << td.is_array << ", " << +td.type << ')';
    return out;
}
