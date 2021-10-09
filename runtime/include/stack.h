#pragma once
#include <cstring>

#define DEF_SIZE 512U
#define GROW_FAC 2U
#define STACK_MAX 64 * UINT8_MAX

struct Stack
{
    size_t count;
    int **data;
    int *back;

    Stack();
    ~Stack();
    int *operator[](const size_t index);
    void push_back(int *cc);
    // returns a pointer to the copy so that the VM
    // can add the pointer to the copy to the Stack of
    // runtime allocated objects
    void pop_back();
    void pop_N(size_t n);

private:
    size_t capacity;
};