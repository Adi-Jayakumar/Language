#include "common.h"

std::unordered_map<std::string, TypeData> &GetTypeNameMap()
{
    static std::unordered_map<std::string, TypeData> TypeNameMap{{"void", {false, 0}}, {"int", {false, 1}}, {"double", {false, 2}}, {"bool", {false, 3}}, {"string", {false, 4}}, {"char", {false, 5}}, {"null_t", {false, 6}}};
    return TypeNameMap;
}

std::unordered_map<TypeID, std::string> &GetTypeStringMap()
{
    static std::unordered_map<TypeID, std::string> TypeStringMap{{0, "void"}, {1, "int"}, {2, "double"}, {3, "bool"}, {4, "string"}, {5, "char"}, {6, "null_t"}};
    return TypeStringMap;
}

std::string ToString(const TypeData &td)
{
    std::string aString = (td.isArray ? "Array<" + std::to_string(td.isArray) + ", " : "<");
    return aString + GetTypeStringMap()[td.type] + ">";
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

bool CheckBinaryOperatorUse(const TypeInfo &ti)
{
    TypeData nullT = {false, 6};

    if (ti.t == TokenID::EQ_EQ || ti.t == TokenID::BANG_EQ)
    {
        if (ti.left == nullT)
            return true;
        else if (ti.right == nullT)
            return true;
    }
    return OperatorMap.find(ti) != OperatorMap.end();
}

TypeData GetBinaryOperatorType(const TypeInfo &ti)
{
    TypeData nullT = {false, 6};

    if (ti.t == TokenID::EQ_EQ || ti.t == TokenID::BANG_EQ)
    {
        if (ti.left == nullT)
            return {false, 3};
        else if (ti.right == nullT)
            return {false, 3};
    }
    return OperatorMap.at(ti);
}

bool CheckUnaryOperatorUse(const TypeInfo &ti)
{
    return OperatorMap.find(ti) != OperatorMap.end();
}