#pragma once
#include "typedata.h"
#include <cstring>
#include <string>

#define DEF_SIZE 512U
#define GROW_FAC 2U
#define STACK_MAX 64 * UINT8_MAX

class Stack
{
    char *data;
    size_t capacity;

public:
    size_t size;
    char *top;

    Stack();
    ~Stack();

    int PopInt(const size_t index)
    {
        return *(int *)data[index];
    };

    double PopDouble(const size_t index)
    {
        return *(double *)data[index];
    };

    bool PopBool(const size_t index)
    {
        return *(bool *)data[index];
    }

    char *PopString(const size_t index)
    {
        return data + index;
    }

    char PopChar(const size_t index)
    {
        return *(char *)data[index];
    }

    char *PopStruct(const size_t index)
    {
        return data + index;
    }

    void GrowIfUnableToPush(const size_t bytes);

    void PushInt(const int x)
    {
        GrowIfUnableToPush(INT_SIZE);
        *(int *)top = x;
    };

    void PushDouble(const double x)
    {
        GrowIfUnableToPush(DOUBLE_SIZE);
        *(double *)top = x;
    };

    void PushBool(const bool x)
    {
        GrowIfUnableToPush(BOOL_SIZE);
        *(bool *)top = x;
    };

    void PushString(const std::string &x)
    {
        //TODO error check string size
        GrowIfUnableToPush(STRING_SIZE);
        int len = x.length();
        PushInt(len);

        char *str = new char[len];
        std::memcpy(str, x.data(), len);
        GrowIfUnableToPush(PTR_SIZE);
        *(char **)top = str;
    };

    void PushChar(const char x)
    {
        GrowIfUnableToPush(CHAR_SIZE);
        *(char *)top = x;
    };

    void PushArray(char *x)
    {
        GrowIfUnableToPush(ARRAY_SIZE);
        *(char **)top = x;
    };

    void PushStruct(char *x)
    {
        GrowIfUnableToPush(STRUCT_SIZE);
        *(char **)top = x;
    };

    void PopBytes(const size_t n)
    {
        top -= n;
    };
};