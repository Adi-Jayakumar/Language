#pragma once
#include "runtimeobject.h"
#include <cstring>

#define DEF_SIZE 512U
#define GROW_FAC 2U
#define STACK_MAX 64 * UINT8_MAX

struct Array
{
    size_t count;
    RuntimeObject **data;
    RuntimeObject *back;

    Array();
    ~Array();
    RuntimeObject *operator[](const size_t index);
    void push_back(RuntimeObject *cc);
    void push_back_copy(RuntimeObject rto);
    void pop_back();
    void pop_N(size_t n);

private:
    size_t capacity;
};