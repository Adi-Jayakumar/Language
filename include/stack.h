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
    // returns a pointer to the copy so that the VM
    // can add the pointer to the copy to the Array of
    // runtime allocated objects
    void push_back_copy(RuntimeObject *);
    void pop_back();
    void pop_N(size_t n);

private:
    size_t capacity;
};