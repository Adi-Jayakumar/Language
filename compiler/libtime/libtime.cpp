#include "libtime.h"

static std::chrono::_V2::system_clock::time_point last;

extern "C" Object *StartTime(VM *vm, Object **obj)
{
    last = std::chrono::system_clock::now();
    return vm->NewInt(0);
}

extern "C" Object *EndTime(VM *vm, Object **obj)
{
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
    return vm->NewInt(diff.count());
}