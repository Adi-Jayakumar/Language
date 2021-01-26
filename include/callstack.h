#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

struct CallFrame
{
    size_t retIndex;
    size_t retChunk;
    size_t valStackMin;
};

struct CallStack
{
    std::vector<CallFrame> s;
    CallStack() = default;

    void Push(CallFrame cf);
    CallFrame Top();
    void Pop();

    bool Empty();
    size_t Size();

    CallFrame operator[](size_t i);
};