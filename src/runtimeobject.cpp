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

Object *CreateBool(bool b)
{
    return new Bool(b);
}

std::string Bool::ToString()
{
    return b ? "true" : "false";
}

Object *CreateArray(Object **arr, size_t len)
{
    return new Array(arr, len);
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

Object *CreateStruct(Object **strct, size_t len, TypeID type)
{
    return new Struct(strct, len, type);
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

Object *CreateChar(char c)
{
    return new Char(c);
}

std::string Char::ToString()
{
    return std::string(1, c);
}

Object *CreateString(char *str, size_t len)
{
    return new String(str, len);
}

std::string String::ToString()
{
    return std::string(str, len);
}

Object *CreateNull_T()
{
    return new Null_T();
}

std::string Null_T::ToString()
{
    return "null";
}

bool IsNull_T(Object *o)
{
    return dynamic_cast<Null_T *>(o) == nullptr;
}

// GETTERS
int GetInt(Object *o)
{
    return static_cast<Int *>(o)->i;
}

double GetDouble(Object *o)
{
    return static_cast<Double *>(o)->d;
}

bool GetBool(Object *o)
{
    return static_cast<Bool *>(o)->b;
}

size_t GetArrayLength(Object *o)
{
    return static_cast<Array *>(o)->size;
}

Object **GetArray(Object *o)
{
    return static_cast<Array *>(o)->arr;
}

size_t GetNumStructMembers(Object *o)
{
    return static_cast<Struct *>(o)->size;
}

Object **GetStructMembers(Object *o)
{
    return static_cast<Struct *>(o)->arr;
}

TypeID GetStructTypes(Object *o)
{
    return static_cast<Struct *>(o)->type;
}

char GetChar(Object *o)
{
    return static_cast<Char *>(o)->c;
}

char *GetString(Object *o)
{
    return static_cast<String *>(o)->str;
}

size_t GetStringLen(Object *o)
{
    return static_cast<String *>(o)->len;
}