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

enum class GCSate : uint8_t
{
    UNMARKED,
    MARKED,
    FREED,
};

std::string ToString(const GCSate &gcs);
std::ostream &operator<<(std::ostream &out, const GCSate &gcs);

enum class RuntimeType : uint8_t
{
    NULL_T,
    INT,
    DOUBLE,
    BOOL,
    ARRAY,
    STRING,
    CHAR,
    STRUCT,
};

std::string ToString(const RuntimeType &gcs);
std::ostream &operator<<(std::ostream &out, const RuntimeType &gcs);

struct RuntimeObject
{
    GCSate state;
    RuntimeType t;
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
    RuntimeObject(RuntimeType, std::string);
    RuntimeObject(int);
    RuntimeObject(double);
    RuntimeObject(bool);

    // array case
    RuntimeObject(RuntimeType, size_t);
    RuntimeObject(RTArray);

    // string case
    RuntimeObject(std::string);
    RuntimeObject(char *);
    RuntimeObject(RTString);

    // char case
    RuntimeObject(char);
};

void CopyRTO(RuntimeObject *, const RuntimeObject &);

std::string ToString(const RuntimeObject &cc);
bool IsTruthy(const RuntimeObject &cc);

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc);