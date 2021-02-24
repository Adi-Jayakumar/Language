#pragma once
#include "common.h"
#include <cstring>

struct RuntimeObject;

struct CCArray
{
    size_t size;
    RuntimeObject *data;
};

struct CCString
{
    size_t len;
    char *data;
};

enum class GCSate
{
    UNMARKED,
    MARKED,
    FREED,
};

struct RuntimeObject
{
    GCSate state;
    TypeData t;
    union combo
    {
        int i;
        double d;
        bool b;
        CCArray arr;
        CCString str;
        char c;
    } as;

    RuntimeObject() = default;
    RuntimeObject(TypeData, std::string);
    RuntimeObject(int);
    RuntimeObject(double);
    RuntimeObject(bool);

    // array case
    RuntimeObject(TypeData &, size_t);
    RuntimeObject(CCArray);

    // string case
    RuntimeObject(std::string);
    RuntimeObject(char *);
    RuntimeObject(CCString);

    // char case
    RuntimeObject(char);
};

std::string ToString(const RuntimeObject &cc);
bool IsTruthy(const RuntimeObject &cc);

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc);