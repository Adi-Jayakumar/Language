#include "runtimefunc.h"

RuntimeFunction::RuntimeFunction(const Chunk &c)
{
    arity = c.arity;
    code = c.code;
    values = c.values;

    ints = c.ints;
    doubles = c.doubles;
    bools = c.bools;
    strings = c.strings;
    chars = c.chars;
}