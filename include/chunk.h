#pragma once
#include "op.h"
#include "runtimeobject.h"

struct CTVarID
{
    std::string name;
    uint8_t depth;
    uint8_t index;
};

struct Chunk
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<RuntimeObject> values;
    std::vector<CTVarID> vars;
    uint8_t depth = 1;
    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
    size_t ChunkResolveVariable(std::string &);
    void CleanUpVariables();
};
