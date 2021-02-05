#pragma once
#include "callstack.h"
#include "compileconst.h"
#include "compiler.h"
#include "stack.h"

struct VM
{
    /*
        When executing multiple Chunks the codes GET_V and
        VAR_D provide relative 'addresses' of where their
        variables should go/are so we update this offset
        everytime we switch Chunks
    */

    std::vector<Chunk> functions;
    std::vector<CompileConst> globals;

    // instruction pointer
    size_t ip;

    // Call stack
    CallFrame *cs;

    // current CallFrame
    CallFrame *curCF;

    // current Chunk index
    size_t curChunk;

    std::vector<CompileConst> constants;

    Array stack;

    VM(std::vector<Chunk> &, size_t);
    ~VM();

    void PrintStack();

    void SetChunk(size_t);

    void Jump(size_t jump);
    void ExecuteCurrentChunk();
    void ExecuteInstruction();

    void NativeFunction(size_t index);
};