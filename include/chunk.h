#pragma once
#include "idstructs.h"
#include "op.h"
#include "runtimeobject.h"

struct Chunk
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<Object *> values;

    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
};
