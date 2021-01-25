#pragma once
#include "compileconst.h"
#include "op.h"

struct CTVarID
{
    std::string name;
    uint8_t depth;
    uint8_t index;
};

struct Chunk
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    std::vector<CTVarID> vars;
    uint8_t depth = 1;
    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
    size_t ChunkResolveVariable(std::string &);
    void CleanUpVariables();
};
