#include "typedata.h"

bool operator==(const TypeData &left, const TypeData &right)
{
    bool cand = (left.type == right.type) && (left.is_array == right.is_array);

    if (!cand)
        return false;

    if (left.tmps.size() != right.tmps.size())
        return false;

    for (size_t i = 0; i < left.tmps.size(); i++)
    {
        if (operator!=(left.tmps[i], right.tmps[i]))
            return false;
    }

    return true;
}

bool operator!=(const TypeData &left, const TypeData &right)
{
    return !operator==(left, right);
}

std::ostream &operator<<(std::ostream &out, const TypeData td)
{
    out << '(' << td.is_array << ", " << +td.type << ')';
    return out;
}
