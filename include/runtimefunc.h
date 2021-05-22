#pragma once
#include "chunk.h"

struct RuntimeFunction
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<RuntimeObject*> values;
    RuntimeFunction(const Chunk &c);
};