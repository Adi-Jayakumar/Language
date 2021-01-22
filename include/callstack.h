#pragma once
#include <cstdint>
#include <vector>
#include <iostream>

struct CallFrame
{
    size_t retIndex;
    size_t retChunk;
    size_t valStackMin;
    size_t varListMin;
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