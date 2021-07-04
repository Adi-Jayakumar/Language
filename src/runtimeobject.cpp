#include "runtimeobject.h"

Object *CreateInt(int i)
{
    return new Int(i);
}

std::string Int::ToString()
{
    return std::to_string(i);
}

Object *CreateDouble(double d)
{
    return new Double(d);
}

std::string Double::ToString()
{
    return std::to_string(d);
}

Object *CreateArray(size_t len, Object **arr)
{
    return new Array(len, arr);
}

std::string Array::ToString()
{
    std::ostringstream out;
    out << "{";

    for (size_t i = 0; i < size - 1; i++)
        out << arr[i]->ToString() << ", ";

    out << arr[size - 1]->ToString() << "}";
    return out.str();
}

Object *CreateStruct(size_t len, Object **strct, TypeID type)
{
    return new Struct(len, strct, type);
}

std::string Struct::ToString()
{
    std::ostringstream out;
    out << "{";

    for (size_t i = 0; i < size - 1; i++)
        out << arr[i]->ToString() << ", ";

    out << arr[size - 1]->ToString() << "}";
    return out.str();
}