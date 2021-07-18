#include <cstddef>
#include <cstdint>

class ThrowInfo
{
public:
    // type of catch variable type
    bool isArray;
    uint8_t type;
    // function to return to
    uint8_t func;
    // index of the function's code with
    // the beginning of the catch statement
    uint8_t index;
    // index on the call stack to pop call frames
    // off until
    size_t callStackIndex;

    ThrowInfo() = default;
    ThrowInfo(bool _isArray, uint8_t _type, uint8_t _func, uint8_t _index) : isArray(_isArray), type(_type), func(_func), index(_index), callStackIndex(0){};
};