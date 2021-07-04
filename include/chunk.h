#pragma once
#include "idstructs.h"
#include "op.h"
#include "runtimeobject.h"

struct Chunk
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<Object *> values;

    std::vector<int> ints;
    std::vector<double> doubles;
    std::vector<bool> bools;
    std::vector<std::string> strings;
    std::vector<char> chars;

    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
};
