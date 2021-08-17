#include "libmath.h"

extern "C" Object *Sin(VM *vm, Object **args)
{
    Object *obj = vm->NewDouble(sin(GetDouble(args[0])));
    return obj;
}

extern "C" Object *Cos(VM *vm, Object **args)
{
    Object *obj = vm->NewDouble(cos(GetDouble(args[0])));
    return obj;
}

extern "C" Object *Tan(VM *vm, Object **args)
{
    Object *obj = vm->NewDouble(tan(GetDouble(args[0])));
    return obj;
}

extern "C" Object *GetPi(VM *vm, Object **)
{
    Object *obj = vm->NewDouble(3.1415926535);
    return obj;
}

extern "C" Object *DoNothing(VM *vm, Object **args)
{
    return nullptr;
}

extern "C" Object *EuclideanDist(VM *vm, Object **args)
{
    double x = GetDouble(args[0]);
    double y = GetDouble(args[1]);
    Object *obj = vm->NewDouble(fabs(y - x));
    return obj;
}