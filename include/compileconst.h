#pragma once
#include "common.h"
#include <cstring>

struct CompileConst;

struct CCArray
{
    size_t size;
    CompileConst *data;
};

struct CCString
{
    size_t len;
    char *data;
};

struct CompileConst
{
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

    CompileConst() = default;
    CompileConst(TypeData, std::string);
    CompileConst(int);
    CompileConst(double);
    CompileConst(bool);

    // array case
    CompileConst(TypeData &, size_t);
    CompileConst(CCArray);

    // string case
    CompileConst(std::string);
    CompileConst(char *);
    CompileConst(CCString);

    // char case
    CompileConst(char);
};

std::string ToString(const CompileConst &cc);
bool IsTruthy(const CompileConst &cc);

std::ostream &operator<<(std::ostream &out, const CompileConst &cc);