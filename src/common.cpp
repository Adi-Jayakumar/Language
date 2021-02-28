#include "common.h"

std::unordered_map<std::string, TypeData> &GetTypeNameMap()
{
    static std::unordered_map<std::string, TypeData> TypeNameMap{{"void", {false, 0}}, {"int", {false, 1}}, {"double", {false, 2}}, {"bool", {false, 3}}, {"string", {false, 4}}, {"char", {false, 5}}};
    return TypeNameMap;
}

std::unordered_map<TypeID, std::string> &GetTypeStringMap()
{
    static std::unordered_map<TypeID, std::string> TypeStringMap{{0, "void"}, {1, "int"}, {2, "double"}, {3, "bool"}, {4, "string"}, {5, "char"}};
    return TypeStringMap;
}

std::string ToString(const TypeData &td)
{
    std::string aString = (td.isArray ? "Array" : "");
    return aString + "<" + GetTypeStringMap()[td.type] + ">";
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