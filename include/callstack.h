#pragma once
#include <cstddef>

class CallFrame
{
public:
    size_t retIndex;
    size_t retChunk;
    size_t valStackMin;

    CallFrame(size_t _retIndex, size_t _retChunk, size_t _valStackMin) : retIndex(_retIndex), retChunk(_retChunk), valStackMin(_valStackMin){};
};