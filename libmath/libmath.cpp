#include "libmath.h"

extern "C" Object *Sin(Object **args)
{
    return CreateDouble(sin(GetDouble(args[0])));
}

extern "C" Object *Cos(Object **args)
{
    return CreateDouble(cos(GetDouble(args[0])));
}

extern "C" Object *Tan(Object **args)
{
    return CreateDouble(tan(GetDouble(args[0])));
}

extern "C" Object *GetPi(Object **)
{
    return CreateDouble(3.1415926535);
}

extern "C" Object *DoNothing(Object **args)
{
    return nullptr;
}

extern "C" Object *EuclideanDist(Object **args)
{
    double x = GetDouble(args[0]);
    double y = GetDouble(args[1]);
    return CreateDouble(fabs(y - x));
}