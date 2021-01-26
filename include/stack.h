#pragma once
#include "compileconst.h"

struct Stack
{
    std::vector<CompileConst> s;
    Stack() = default;
    Stack(size_t);

    void Push(CompileConst cc);
    CompileConst Top();
    void Pop();

    bool Empty();
    size_t Size();

    CompileConst &operator[](size_t i);
};