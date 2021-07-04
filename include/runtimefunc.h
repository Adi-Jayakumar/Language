#pragma once
#include "chunk.h"

struct RuntimeFunction
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<Object *> values;

    std::vector<int> ints;
    std::vector<double> doubles;
    std::vector<bool> bools;
    std::vector<std::string> strings;
    std::vector<char> chars;

    RuntimeFunction(const Chunk &c);
};