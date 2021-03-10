#pragma once
#include "common.h"
#include <cstring>

struct RuntimeObject;

struct RTArray
{
    size_t size;
    RuntimeObject **data;
};

struct RTString
{
    size_t len;
    char *data;
};

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
    RuntimeType t;
    union combo
    {
        int i;
        double d;
        bool b;
        // also for structs
        RTArray arr;
        RTString str;
        char c;
    } as;

    RuntimeObject();
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

RuntimeObject *GetNull();

void CopyRTO(RuntimeObject *, const RuntimeObject &);

std::string ToString(const RuntimeObject &cc);
bool IsTruthy(const RuntimeObject &cc);

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc);