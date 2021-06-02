#include "libmath.h"

extern "C"
{
    LibInfo FunctionPrototypes()
    {
        char *info[] = {"Sin : double -> double",
                        "Cos : double -> double",
                        "Tan : double -> double"};

        return {3, info};
    }

    RuntimeObject *Sin(RuntimeObject *x)
    {
        double sin_x = sin(GetDouble(x));
        return CreateDouble(sin_x);
    }

    RuntimeObject *Cos(RuntimeObject *x)
    {
        double cos_x = cos(GetDouble(x));
        return CreateDouble(cos_x);
    }

    RuntimeObject *Tan(RuntimeObject *x)
    {
        double tan_x = tan(GetDouble(x));
        return CreateDouble(tan_x);
    }
}