#include "common.h"

bool operator==(const TypeData &left, const TypeData &right)
{
    return (left.type == right.type) && (left.is_array == right.is_array);
}

bool operator!=(const TypeData &left, const TypeData &right)
{
    return (left.type != right.type) || (left.is_array != right.is_array);
}