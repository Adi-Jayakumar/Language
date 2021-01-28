#pragma once
#include "compileconst.h"
#include <cstring>

#define DEF_SIZE 512U
#define GROW_FAC 2U

struct Array
{
    size_t count;
    CompileConst *data;
    CompileConst *back;

    Array();
    ~Array();
    CompileConst &operator[](const size_t index);
    void push_back(CompileConst cc);
    void pop_back();

private:
    size_t capacity;
};