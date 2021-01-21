#pragma once
#include "compileconst.h"
#include "op.h"

struct CTVarID
{
    std::string name;
    uint16_t depth;
    uint16_t index;
};

struct Chunk
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    std::vector<CTVarID> vars;
    uint16_t depth = 1;
    uint16_t numPops = 0;
    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
    size_t ResolveVariable(std::string &);
    void CleanUpVariables();
};
