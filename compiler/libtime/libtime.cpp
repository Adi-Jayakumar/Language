#include "libtime.h"

extern "C" Object *StartTime(VM *vm, Object **obj)
{
    LastTime = time(NULL);
    return vm->NewInt(LastTime);
}