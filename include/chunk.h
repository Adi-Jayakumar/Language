#pragma once
#include "op.h"
#include "runtimeobject.h"

struct CTVarID
{
    std::string name;
    uint8_t depth;
    uint8_t index;
    bool isStructMember = false;
};

struct Chunk
{
    uint8_t arity;
    std::vector<Op> code;
    std::vector<RuntimeObject> values;
    std::vector<CTVarID> vars;

    uint8_t depth = 1;
    // used to calculate the offsets for struct member accesses
    uint8_t varOffset = 0;
    bool isAssign = false;

    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
    size_t ChunkResolveVariable(std::string &);
    void CleanUpVariables();
    void CleanUpVariablesNoPOP();
};
