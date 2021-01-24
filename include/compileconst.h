#pragma once
#include "common.h"

struct CompileConst
{
    TypeID type;
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
    CompileConst() = default;
    CompileConst(TypeID, std::string);
    CompileConst(int);
    CompileConst(double);
    CompileConst(bool);
};

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

struct CompileVar
{
    std::string name;
    uint8_t index;
    CompileVar() = default;
    CompileVar(std::string &, uint8_t);
};
std::ostream &operator<<(std::ostream &out, const CompileVar &cv);