#pragma once
#include "common.h"

struct CompileConst;

struct CCArray
{
    size_t size;
    CompileConst *data;
};

struct CompileConst
{
    TypeID type;
    union combo
    {
        int i;
        double d;
        bool b;
        CCArray arr;
    } as;
    CompileConst() = default;
    CompileConst(TypeID, std::string);
    CompileConst(int);
    CompileConst(double);
    CompileConst(bool);
    // array case
    CompileConst(size_t);
};

std::string ToString(const CompileConst &cc);
bool IsTruthy(const CompileConst &cc);

std::ostream &operator<<(std::ostream &out, const CompileConst &cc);

CompileConst operator+(const CompileConst &left, const CompileConst &right);
CompileConst operator-(const CompileConst &left, const CompileConst &right);
CompileConst operator*(const CompileConst &left, const CompileConst &right);
CompileConst operator/(const CompileConst &left, const CompileConst &right);

CompileConst operator>(const CompileConst &left, const CompileConst &right);
CompileConst operator<(const CompileConst &left, const CompileConst &right);
CompileConst operator>=(const CompileConst &left, const CompileConst &right);
CompileConst operator<=(const CompileConst &left, const CompileConst &right);
CompileConst operator==(const CompileConst &left, const CompileConst &right);
CompileConst operator!=(const CompileConst &left, const CompileConst &right);