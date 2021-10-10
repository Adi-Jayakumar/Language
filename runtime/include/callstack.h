#pragma once
#include <cstddef>

class CallFrame
{
public:
    size_t ret_index;
    size_t ret_function;
    size_t val_stack_min;
    //TODO - Include routine to return to

    CallFrame() = default;
    CallFrame(size_t _ret_index, size_t _ret_function, size_t _val_stack_min) : ret_index(_ret_index), ret_function(_ret_function), val_stack_min(_val_stack_min){};
};