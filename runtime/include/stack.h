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
    oprand_t capacity;
    char *top;
    oprand_t size;

public:
    Stack();
    ~Stack();

    char *GetTop() { return top; };
    oprand_t GetSize() { return size; };
    void ReduceSize(const oprand_t x) { size -= x; };

    int GetInt(const oprand_t index)
    {
        return *(int *)&data[index];
    };

    void SetInt(const oprand_t index, const int x)
    {
        *(int *)&data[index] = x;
    };

    int PopInt()
    {
        top -= INT_SIZE;
        return *(int *)top;
    };

    int PeekInt()
    {
        char *copy = top;
        copy -= INT_SIZE;
        return *(int *)copy;
    };

    double GetDouble(const oprand_t index)
    {
        return *(double *)&data[index];
    };

    void SetDouble(const oprand_t index, const double x)
    {
        *(double *)&data[index] = x;
    };

    double PopDouble()
    {
        top -= DOUBLE_SIZE;
        return *(double *)top;
    };

    double PeekDouble()
    {
        char *copy = top;
        copy -= DOUBLE_SIZE;
        return *(double *)copy;
    };

    bool GetBool(const oprand_t index)
    {
        return *(bool *)&data[index];
    };

    void SetBool(const oprand_t index, const bool x)
    {
        *(bool *)&data[index] = x;
    };

    bool PopBool()
    {
        top -= BOOL_SIZE;
        return *(bool *)top;
    };

    bool PeekBool()
    {
        char *copy = top;
        copy -= BOOL_SIZE;
        return *(bool *)copy;
    };

    char *GetString(const oprand_t index)
    {
        return data + index;
    };

    void SetString(const oprand_t index, char *str, int len)
    {
        *(char **)&data[index] = str;
        *(int *)&data[index + INT_SIZE] = len;
    };

    char *PopString()
    {
        top -= STRING_SIZE;
        return top;
    };

    char *PeekString()
    {
        char *copy = top;
        copy -= STRING_SIZE;
        return copy;
    };

    char GetChar(const oprand_t index)
    {
        return *(char *)&data[index];
    };

    void SetChar(const oprand_t index, char x)
    {
        *(char *)&data[index] = x;
    }

    char PopChar()
    {
        top -= CHAR_SIZE;
        return *(char *)top;
    };

    char PeekChar()
    {
        char *copy = top;
        copy -= CHAR_SIZE;
        return *(char *)top;
    };

    char *GetStruct(const oprand_t index)
    {
        return data + index;
    };

    void SetStruct(const oprand_t index, char *strct)
    {
        *(char **)&data[index] = strct;
    }

    char *PopStruct()
    {
        top -= STRUCT_SIZE;
        return *(char **)top;
    };

    char *PeekStruct()
    {
        char *copy = top;
        copy -= STRUCT_SIZE;
        return *(char **)copy;
    };

    char *PopPtr()
    {
        top -= STRUCT_SIZE;
        return *(char **)top;
    };

    char *PeekPtr()
    {
        return top - STRUCT_SIZE;
    };

    void GrowIfUnableToPush(const oprand_t bytes);

    void PushInt(const int x)
    {
        GrowIfUnableToPush(INT_SIZE);
        *(int *)top = x;
        top += INT_SIZE;
    };

    void PushOprandT(const oprand_t x)
    {
        GrowIfUnableToPush(sizeof(oprand_t));
        *(oprand_t *)top = x;
        top += sizeof(oprand_t);
    }

    void PushDouble(const double x)
    {
        GrowIfUnableToPush(DOUBLE_SIZE);
        *(double *)top = x;
        top += DOUBLE_SIZE;
    };

    void PushBool(const bool x)
    {
        GrowIfUnableToPush(BOOL_SIZE);
        *(bool *)top = x;
        top += BOOL_SIZE;
    };

    void PushString(const std::string &x)
    {
        int len = x.length();
        char *str = new char[len];

        std::memcpy(str, x.data(), len);
        GrowIfUnableToPush(PTR_SIZE);
        *(char **)top = str;

        PushInt(len);
        top += STRING_SIZE;
    };

    void PushString(char *str, int len)
    {
        GrowIfUnableToPush(STRING_SIZE);
        *(char **)top = str;
        PushInt(len);
    }

    void PushChar(const char x)
    {
        GrowIfUnableToPush(CHAR_SIZE);
        *(char *)top = x;
        top += CHAR_SIZE;
    };

    void PushArray(char *x)
    {
        GrowIfUnableToPush(ARRAY_SIZE);
        *(char **)top = x;
        top += ARRAY_SIZE;
    };

    void PushStruct(char *x)
    {
        GrowIfUnableToPush(STRUCT_SIZE);
        *(char **)top = x;
        top += STRUCT_SIZE;
    };

    void PushPtr(char *x)
    {
        GrowIfUnableToPush(ARRAY_SIZE);
        *(char **)top = x;
        top += ARRAY_SIZE;
    }

    void PopBytes(const oprand_t n)
    {
        top -= n;
    };
};