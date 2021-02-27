#include "runtimeobject.h"
#define DUMMYCC RuntimeObject(255, "") // to silence compiler warnings -- never returned due to typechecking

#define PRINT_ARRAY()                          \
    do                                         \
    {                                          \
        RTArray arr = cc.as.arr;               \
        out = out + "{";                       \
                                               \
        for (size_t i = 0; i < arr.size; i++)  \
        {                                      \
            out = out + ToString(arr.data[i]); \
                                               \
            if (i != arr.size - 1)             \
                out = out + ", ";              \
        }                                      \
                                               \
        out = out + "}";                       \
    } while (false)

std::string ToString(const RuntimeObject &cc)
{
    if (cc.state == GCSate::FREED)
        return "";

    std::string out;

    switch (cc.t.type)
    {
    case 0:
    {
        if (cc.t.isArray)
            PRINT_ARRAY();
        break;
    }
    case 1:
    {
        if (cc.t.isArray)
            PRINT_ARRAY();
        else
            out = out + std::to_string(cc.as.i);
        break;
    }
    case 2:
    {
        if (cc.t.isArray)
            PRINT_ARRAY();
        else
            out = out + std::to_string(cc.as.d);
        break;
    }
    case 3:
    {
        if (cc.t.isArray)
            PRINT_ARRAY();
        else
        {
            if (cc.as.b)
                out = out + "true";
            else
                out = out + "false";
        }
        break;
    }
    case 4:
    {
        if (cc.t.isArray)
            PRINT_ARRAY();
        else
            out = out + cc.as.str.data;
        break;
    }
    case 5:
    {
        if (cc.t.isArray)
            PRINT_ARRAY();
        else
            out = out + cc.as.c;
        break;
    }
    }
    return out;
}

bool IsTruthy(const RuntimeObject &cc)
{
    switch (cc.t.type)
    {
    case 1:
    {
        return cc.as.i;
    }
    case 2:
    {
        return cc.as.d;
    }
    case 3:
    {
        return cc.as.b;
    }
    }
    return false;
}

RuntimeObject::RuntimeObject(TypeData _type, std::string literal)
{
    state = GCSate::MARKED;
    t = _type;
    switch (t.type)
    {
    // sentinel null value
    case 0:
    {
        as.i = 0;
        break;
    }
    case 1:
    {
        as.i = std::stoi(literal);
        break;
    }
    case 2:
    {
        as.d = std::stod(literal);
        break;
    }
    case 3:
    {
        if (literal == "true")
            as.b = true;
        else
            as.b = false;
        break;
    }
    case 4:
    {
        size_t stringLen = literal.size();
        const char *asPtr = literal.c_str();

        RTString str;

        str.len = stringLen;
        str.data = (char *)malloc((stringLen + 1) * sizeof(char));
        strcpy(str.data, asPtr);

        as.str = str;
        break;
    }
    case 5:
    {
        // temporary until escaped chars implemented
        as.c = literal[0];
        break;
    }
    }
}

RuntimeObject::RuntimeObject(int _i)
{
    state = GCSate::MARKED;
    t = {false, 1};
    as.i = _i;
}

RuntimeObject::RuntimeObject(double _d)
{
    state = GCSate::MARKED;
    t = {false, 2};
    as.d = _d;
}

RuntimeObject::RuntimeObject(bool _b)
{
    state = GCSate::MARKED;
    t = {false, 3};
    as.b = _b;
}

RuntimeObject::RuntimeObject(TypeData _type, size_t _size)
{
    state = GCSate::MARKED;
    t = _type;
    as.arr.data = (RuntimeObject *)malloc(_size * sizeof(RuntimeObject));
    as.arr.size = _size;
}

RuntimeObject::RuntimeObject(RTArray _arr)
{
    state = GCSate::MARKED;
    t = {true, 1};
    as.arr = _arr;
}

RuntimeObject::RuntimeObject(std::string _str)
{
    state = GCSate::MARKED;
    t = {false, 4};
    size_t stringLen = _str.size();
    const char *asPtr = _str.c_str();

    RTString str;

    str.len = stringLen;
    str.data = (char *)malloc((stringLen + 1) * sizeof(char));
    strcpy(str.data, asPtr);

    as.str = str;
}

RuntimeObject::RuntimeObject(char *_str)
{
    state = GCSate::MARKED;
    t = {false, 4};
    size_t stringLen = strlen(_str);

    RTString str;

    str.len = stringLen;
    str.data = (char *)malloc((stringLen + 1) * sizeof(char));
    strcpy(str.data, _str);

    as.str = str;
}

RuntimeObject::RuntimeObject(RTString _str)
{
    state = GCSate::MARKED;
    t = {false, 4};
    as.str = _str;
}

RuntimeObject::RuntimeObject(char c)
{
    state = GCSate::MARKED;
    t = {false, 5};
    as.c = c;
}

void CopyRTO(RuntimeObject *copy, const RuntimeObject &rto)
{
    copy->state = rto.state;
    copy->t = rto.t;
    copy->as = rto.as;
}

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc)
{
    out << ToString(cc);
    return out;
}