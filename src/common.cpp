#include "common.h"

std::string ToString(const TypeData &td)
{
    std::string aString = (td.isArray ? "Array" : "");
    return aString + "<" + TypeStringMap[td.type] + ">";
}

std::ostream &operator<<(std::ostream &out, const TypeData &td)
{
    out << ToString(td);
    return out;
}

bool operator==(const TypeData &left, const TypeData &right)
{
    return (left.type == right.type) && (left.isArray == right.isArray);
}

bool operator!=(const TypeData &left, const TypeData &right)
{
    return (left.type != right.type) || (left.isArray != right.isArray);
}

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}