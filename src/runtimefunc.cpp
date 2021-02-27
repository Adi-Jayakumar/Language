#include "runtimefunc.h"

RuntimeFunction::RuntimeFunction(const Chunk &c)
{
    arity = c.arity;
    code = c.code;
    values = c.constants;
}