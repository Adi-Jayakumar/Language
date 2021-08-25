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
    return aString + GetTypeStringMap()[td.type] + (td.isArray ? ">" : "");
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

bool CheckOperatorUse(const TypeData &left, const TokenID &op, const TypeData &right)
{
    return OperatorMap.find(TypeInfo(left, op, right)) != OperatorMap.end();
}

TypeData OperatorResult(const TypeData &left, const TokenID &op, const TypeData &right)
{
    return OperatorMap.at(TypeInfo(left, op, right));
}