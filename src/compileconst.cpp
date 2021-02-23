#include "compileconst.h"
#define DUMMYCC CompileConst(255, "") // to silence compiler warnings -- never returned due to typechecking

std::string ToString(const CompileConst &cc)
{
    switch (cc.t.type)
    {
    case 1:
    {
        return std::to_string(cc.as.i);
    }
    case 2:
    {
        return std::to_string(cc.as.d);
    }
    case 3:
    {
        if (cc.as.b)
            return "true";
        else
            return "false";
    }
        // case 4:
        // {
        //     return std::string(cc.as.str.data);
        // }
    }
    return "";
}

bool IsTruthy(const CompileConst &cc)
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

CompileConst::CompileConst(TypeData _type, std::string literal)
{
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
        t = {false, 4};
        size_t stringLen = literal.size();
        const char *asPtr = literal.c_str();

        CCString str;

        str.len = stringLen;
        str.data = (char *)malloc((stringLen + 1) * sizeof(char));
        strcpy(str.data, asPtr);

        as.str = str;
        break;
    }
    }
}

CompileConst::CompileConst(int _i)
{
    t = {false, 1};
    as.i = _i;
}

CompileConst::CompileConst(double _d)
{
    t = {false, 2};
    as.d = _d;
}

CompileConst::CompileConst(bool _b)
{
    t = {false, 3};
    as.b = _b;
}

CompileConst::CompileConst(TypeData &_type, size_t _size)
{
    t = _type;
    as.arr.data = (CompileConst *)malloc(_size * sizeof(CompileConst));
    as.arr.size = _size;
}

CompileConst::CompileConst(CCArray _arr)
{
    t = {true, 0};
    as.arr = _arr;
}

CompileConst::CompileConst(std::string _str)
{
    t = {false, 4};
    size_t stringLen = _str.size();
    const char *asPtr = _str.c_str();

    CCString str;

    str.len = stringLen;
    str.data = (char *)malloc((stringLen + 1) * sizeof(char));
    strcpy(str.data, asPtr);

    as.str = str;
}

CompileConst::CompileConst(char *_str)
{
    t = {false, 4};
    size_t stringLen = strlen(_str);

    CCString str;

    str.len = stringLen;
    str.data = (char *)malloc((stringLen + 1) * sizeof(char));
    strcpy(str.data, _str);

    as.str = str;
}

CompileConst::CompileConst(CCString _str)
{
    t = {false, 4};
    as.str = _str;
}

#define PRINT_ARRAY()                     \
    CCArray arr = cc.as.arr;              \
    out << "{";                           \
                                          \
    for (size_t i = 0; i < arr.size; i++) \
    {                                     \
        out << arr.data[i];               \
                                          \
        if (i != arr.size - 1)            \
            out << ", ";                  \
    }                                     \
                                          \
    out << "}"

std::ostream &operator<<(std::ostream &out, const CompileConst &cc)
{
    switch (cc.t.type)
    {
    case 1:
    {
        if (cc.t.isArray)
        {
            PRINT_ARRAY();
        }
        else
            out << cc.as.i;
        break;
    }
    case 2:
    {
        if (cc.t.isArray)
        {
            PRINT_ARRAY();
        }
        else
            out << cc.as.d;
        break;
    }
    case 3:
    {
        if (cc.t.isArray)
        {
            PRINT_ARRAY();
        }
        else
        {
            if (cc.as.b)
                out << "true";
            else
                out << "false";
        }
        break;
    }
    case 4:
    {
        if (cc.t.isArray)
        {
            PRINT_ARRAY();
        }
        else
            out << "\"" << cc.as.str.data << "\"";
        break;
    }
    }
    return out;
}