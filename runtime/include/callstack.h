#pragma once
#include <cstddef>

class CallFrame
{
public:
    size_t retIndex;
    size_t retFunction;
    size_t valStackMin;

    CallFrame() = default;
    CallFrame(size_t _retIndex, size_t _retFunction, size_t _valStackMin) : retIndex(_retIndex), retFunction(_retFunction), valStackMin(_valStackMin){};
};