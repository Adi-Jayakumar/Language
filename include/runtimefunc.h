#pragma once
#include "chunk.h"

struct RuntimeFunction
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<Object *> values;
    RuntimeFunction(const Chunk &c);
};