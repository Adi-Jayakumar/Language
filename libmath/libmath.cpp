#include "libmath.h"

extern "C" Object *Sin(VM *vm, Object **args)
{
    Object *obj = CreateDouble(sin(GetDouble(args[0])));
    vm->AddSingleToHeap(obj);
    return obj;
}

extern "C" Object *Cos(VM *vm, Object **args)
{
    Object *obj = CreateDouble(cos(GetDouble(args[0])));
    vm->AddSingleToHeap(obj);
    return obj;
}

extern "C" Object *Tan(VM *vm, Object **args)
{
    Object *obj = CreateDouble(tan(GetDouble(args[0])));
    vm->AddSingleToHeap(obj);
    return obj;
}

extern "C" Object *GetPi(VM *vm, Object **)
{
    Object *obj = CreateDouble(3.1415926535);
    vm->AddSingleToHeap(obj);
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
    Object *obj = CreateDouble(fabs(y - x));
    vm->AddSingleToHeap(obj);
    return obj;
}