#pragma once
#include "callstack.h"
#include "compileconst.h"
#include "compiler.h"
#include "stack.h"

struct VM
{
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

    void RuntimeError(std::string msg);
    void Jump(size_t jump);
    void ExecuteCurrentChunk();
    void ExecuteInstruction();

    void NativePrint(CompileConst* args, int arity);
    
};