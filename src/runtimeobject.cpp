#include "runtimeobject.h"
#define DUMMYCC RuntimeObject(255, "") // to silence compiler warnings -- never returned due to typechecking

RuntimeObject::RuntimeObject()
{
    t = RuntimeType::NULL_T;
}

RuntimeObject::RuntimeObject(RuntimeType _type, std::string literal)
{
    t = _type;
    if (literal == "null")
        t = RuntimeType::NULL_T;
    switch (t)
    {
    // sentinel null value
    case RuntimeType::NULL_T:
    {
        as.i = 0;
        break;
    }
    case RuntimeType::INT:
    {
        as.i = std::stoi(literal);
        break;
    }
    case RuntimeType::DOUBLE:
    {
        as.d = std::stod(literal);
        break;
    }
    case RuntimeType::BOOL:
    {
        if (literal == "true")
            as.b = true;
        else
            as.b = false;
        break;
    }
    case RuntimeType::STRING:
    {
        size_t stringLen = literal.size();
        const char *asPtr = literal.c_str();

        RTString str;

        str.len = stringLen;
        str.data = (char *)malloc(stringLen + 1);
        strcpy(str.data, asPtr);

        as.str = str;
        break;
    }
    case RuntimeType::CHAR:
    {
        // temporary until escaped chars implemented
        as.c = literal[0];
        break;
    }
    default:
    {
        break;
    }
    }
}

RuntimeObject::RuntimeObject(int _i)
{
    t = RuntimeType::INT;
    as.i = _i;
}

RuntimeObject::RuntimeObject(double _d)
{
    t = RuntimeType::DOUBLE;
    as.d = _d;
}

RuntimeObject::RuntimeObject(bool _b)
{
    t = RuntimeType::BOOL;
    as.b = _b;
}

RuntimeObject::RuntimeObject(RuntimeType _type, size_t _size)
{
    t = _type;
    as.arr.data = (RuntimeObject **)malloc(_size * sizeof(RuntimeObject *));

    for (size_t i = 0; i < _size; i++)
        as.arr.data[i] = GetNull();

    as.arr.size = _size;
}

RuntimeObject::RuntimeObject(std::string _str)
{
    t = RuntimeType::STRING;
    size_t stringLen = _str.size();
    const char *asPtr = _str.c_str();

    RTString str;

    str.len = stringLen;
    str.data = (char *)malloc(stringLen + 1);
    strcpy(str.data, asPtr);

    as.str = str;
}

RuntimeObject::RuntimeObject(RTString _str)
{
    t = RuntimeType::STRING;
    as.str = _str;
}

RuntimeObject::RuntimeObject(char c)
{
    t = RuntimeType::CHAR;
    as.c = c;
}

std::string ToString(const RuntimeType &rtt)
{
    switch (rtt)
    {
    case RuntimeType::NULL_T:
    {
        return "NULL_T";
    }
    case RuntimeType::INT:
    {
        return "INT";
    }
    case RuntimeType::DOUBLE:
    {
        return "DOUBLE";
    }
    case RuntimeType::BOOL:
    {
        return "BOOL";
    }
    case RuntimeType::ARRAY:
    {
        return "ARRAY";
    }
    case RuntimeType::STRING:
    {
        return "STRING";
    }
    case RuntimeType::CHAR:
    {
        return "CHAR";
    }
    case RuntimeType::STRUCT:
    {
        return "STRUCT";
    }
    }
    // should never be reached
    return "UNKNOWN RuntimeType";
}

std::ostream &operator<<(std::ostream &out, const RuntimeType &rtt)
{
    out << ToString(rtt);
    return out;
}

bool IsTruthy(const RuntimeObject &cc)
{
    switch (cc.t)
    {
    case RuntimeType::INT:
    {
        return cc.as.i;
    }
    case RuntimeType::DOUBLE:
    {
        return cc.as.d;
    }
    case RuntimeType::BOOL:
    {
        return cc.as.b;
    }
    default:
    {
        return false;
    }
    }
}

RuntimeObject *GetNull()
{
    static RuntimeObject *nullObj = (RuntimeObject *)malloc(sizeof(RuntimeObject));
    nullObj->t = RuntimeType::NULL_T;
    return nullObj;
}

RuntimeObject *InitialiseRuntimeObject(RuntimeObject *val, int _i)
{
    val->t = RuntimeType::INT;
    val->as.i = _i;
    return val;
}

RuntimeObject *InitialiseRuntimeObject(RuntimeObject *val, double _d)
{
    val->t = RuntimeType::DOUBLE;
    val->as.d = _d;
    return val;
}

RuntimeObject *InitialiseRuntimeObject(RuntimeObject *val, bool _b)
{
    val->t = RuntimeType::BOOL;
    val->as.b = _b;
    return val;
}

// array case
RuntimeObject *InitialiseRuntimeObject(RuntimeObject *val, RuntimeType _t, size_t _size)
{
    val->t = _t;
    val->as.arr.data = (RuntimeObject **)malloc(_size * sizeof(RuntimeObject *));

    for (size_t i = 0; i < _size; i++)
        val->as.arr.data[i] = GetNull();

    val->as.arr.size = _size;
    return val;
}

// string case
RuntimeObject *InitialiseRuntimeObject(RuntimeObject *val, RTString _str)
{
    val->t = RuntimeType::STRING;
    val->as.str = _str;
    return val;
}

// char case
RuntimeObject *InitialiseRuntimeObject(RuntimeObject *val, char _c)
{
    val->t = RuntimeType::CHAR;
    val->as.c = _c;
    return val;
}

void CopyRTO(RuntimeObject *copy, const RuntimeObject &rto)
{
    copy->t = rto.t;
    copy->state = GCState::MARKED;
    if (copy->t == RuntimeType::ARRAY || copy->t == RuntimeType::STRUCT)
    {
        copy->as.arr.data = (RuntimeObject **)malloc(rto.as.arr.size * sizeof(RuntimeObject));
        std::copy(rto.as.arr.data, rto.as.arr.data + rto.as.arr.size, copy->as.arr.data);
        copy->as.arr.size = rto.as.arr.size;
    }
    else
        copy->as = rto.as;
}

#define PRINT_ARRAY()                           \
    do                                          \
    {                                           \
        RTArray arr = rto.as.arr;               \
        out = out + "{";                        \
                                                \
        for (size_t i = 0; i < arr.size; i++)   \
        {                                       \
            out = out + ToString(*arr.data[i]); \
                                                \
            if (i != arr.size - 1)              \
                out = out + ", ";               \
        }                                       \
                                                \
        out = out + "}";                        \
    } while (false)

std::string ToString(const RuntimeObject &rto)
{
    if (rto.state == GCState::FREED)
        return "FREED";
    switch (rto.t)
    {
    case RuntimeType::NULL_T:
    {
        return "NULL_T";
    }
    case RuntimeType::INT:
    {
        return std::to_string(rto.as.i);
    }
    case RuntimeType::DOUBLE:
    {
        return std::to_string(rto.as.d);
    }
    case RuntimeType::BOOL:
    {
        return rto.as.b ? "true" : "false";
    }
    case RuntimeType::ARRAY:
    {
        std::string out;
        PRINT_ARRAY();
        return out;
    }
    case RuntimeType::STRING:
    {
        return std::string(rto.as.str.data, rto.as.str.len);
    }
    case RuntimeType::CHAR:
    {
        return std::string{rto.as.c};
    }
    case RuntimeType::STRUCT:
    {
        std::string out;
        PRINT_ARRAY();
        return out;
    }
    }
    // should never be reached
    return "";
}

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc)
{
    out << ToString(cc);
    return out;
}

std::string ToString(const GCState &rts)
{
    switch (rts)
    {
    case GCState::FREED:
    {
        return "FREED";
    }
    case GCState::MARKED:
    {
        return "MARKED";
    }
    case GCState::UNMARKED:
    {
        return "UNMARKED";
    }
    }
    return "";
}

std::ostream &operator<<(std::ostream &out, const GCState &gcs)
{
    out << ToString(gcs);
    return out;
}