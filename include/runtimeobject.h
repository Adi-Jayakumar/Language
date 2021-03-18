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

enum class GCState : uint8_t
{
    FREED,
    MARKED,
    UNMARKED,
};

std::string ToString(const GCState &rts);
std::ostream &operator<<(std::ostream &out, const GCState &gcs);

struct RuntimeObject
{
    GCState state = GCState::UNMARKED;
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

    // string case
    RuntimeObject(std::string);
    RuntimeObject(RTString);

    // char case
    RuntimeObject(char);
};

RuntimeObject *GetNull();

void CopyRTO(RuntimeObject *, const RuntimeObject &);

std::string ToString(const RuntimeObject &cc);
bool IsTruthy(const RuntimeObject &cc);

std::ostream &operator<<(std::ostream &out, const RuntimeObject &cc);