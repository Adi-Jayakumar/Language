#include "common.h"

std::unordered_map<std::string, TypeData> &GetTypeNameMap()
{
    static std::unordered_map<std::string, TypeData> TypeNameMap{{"void", VOID_TYPE}, {"int", INT_TYPE}, {"double", DOUBLE_TYPE}, {"bool", BOOL_TYPE}, {"string", STRING_TYPE}, {"char", CHAR_TYPE}, {"null_t", NULL_TYPE}};
    return TypeNameMap;
}

std::unordered_map<TypeID, std::string> &GetTypeStringMap()
{
    static std::unordered_map<TypeID, std::string> TypeStringMap{{0, "void"}, {1, "int"}, {2, "double"}, {3, "bool"}, {4, "string"}, {5, "char"}, {6, "null_t"}};
    return TypeStringMap;
}

bool operator==(const TypeData &left, const TypeData &right)
{
    return (left.type == right.type) && (left.isArray == right.isArray);
}

bool operator!=(const TypeData &left, const TypeData &right)
{
    return (left.type != right.type) || (left.isArray != right.isArray);
}

std::string ToString(const TypeData &td)
{
    std::string aString = (td.isArray ? "Array<" + std::to_string(td.isArray) + ", " : "");
    return aString + GetTypeStringMap()[td.type] + (td.isArray ? ">" : " ");
}

std::ostream &operator<<(std::ostream &out, const TypeData &td)
{
    out << ToString(td);
    return out;
}

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

bool CheckBinaryOperatorUse(const TypeInfo &ti)
{
    if (ti.t == TokenID::EQ_EQ || ti.t == TokenID::BANG_EQ)
    {
        if (ti.left == NULL_TYPE)
            return true;
        else if (ti.right == NULL_TYPE)
            return true;
    }
    return OperatorMap.find(ti) != OperatorMap.end();
}

TypeData GetBinaryOperatorType(const TypeInfo &ti)
{
    if (ti.t == TokenID::EQ_EQ || ti.t == TokenID::BANG_EQ)
    {
        if (ti.left == NULL_TYPE)
            return BOOL_TYPE;
        else if (ti.right == NULL_TYPE)
            return BOOL_TYPE;
    }
    return OperatorMap.at(ti);
}

bool CheckUnaryOperatorUse(const TypeInfo &ti)
{
    return OperatorMap.find(ti) != OperatorMap.end();
}