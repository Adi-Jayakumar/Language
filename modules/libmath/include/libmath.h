#include "../../../include/runtimeobject.h"
#include <cmath>

extern "C"
{
    LibInfo FunctionPrototypes();
    RuntimeObject *Sin(RuntimeObject *);
    RuntimeObject *Cos(RuntimeObject *);
    RuntimeObject *Tan(RuntimeObject *);
}