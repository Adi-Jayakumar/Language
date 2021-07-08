#pragma once
#include "idstructs.h"
#include "op.h"
#include "runtimeobject.h"

class Chunk
{
public:
    uint8_t arity;
    std::vector<Op> code;

    std::vector<int> ints;
    std::vector<double> doubles;
    std::vector<bool> bools;
    std::vector<std::string> strings;
    std::vector<char> chars;

    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
};
