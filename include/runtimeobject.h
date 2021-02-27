#pragma once
#include "common.h"
#include <cstring>

struct RuntimeObject;

struct RTArray
{
    size_t size;
    RuntimeObject *data;
};

struct RTString
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

std::string ToString(const GCSate &gcs);
std::ostream &operator<<(std::ostream &out, const GCSate &gcs);

struct RuntimeObject
{
    GCSate state;
    TypeData t;
    union combo
    {
        int i;
        double d;
        bool b;
        RTArray arr;
        RTString str;
        char c;
    } as;

    RuntimeObject() = default;
    RuntimeObject(TypeData, std::string);
    RuntimeObject(int);
    RuntimeObject(double);
    RuntimeObject(bool);

    // array case
    RuntimeObject(TypeData , size_t);
    RuntimeObject(RTArray);

    // string case
    RuntimeObject(std::string);
    RuntimeObject(char *);
    RuntimeObject(RTString);

    // char case
    RuntimeObject(char);
};

std::string ToString(const RuntimeObject &cc);
bool IsTruthy(const RuntimeObject &cc);

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc);