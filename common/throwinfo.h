#include <cstddef>
#include <cstdint>

class ThrowInfo
{
public:
    // type of catch variable type
    bool is_array;
    TypeID type;
    // function to return to
    oprand_t func;
    // index of the function's code with
    // the beginning of the catch statement
    oprand_t index;
    // index on the call stack to pop call frames
    // off until
    size_t callStackIndex;

    ThrowInfo() = default;
    ThrowInfo(bool _is_array, uint8_t _type, uint8_t _func, uint8_t _index) : is_array(_is_array), type(_type), func(_func), index(_index), callStackIndex(0){};
};