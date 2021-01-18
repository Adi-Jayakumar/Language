#pragma once
#include "compiler.h"
#include "stack.h"
#include "compileconst.h"



struct VM
{
    /*
        When executing multiple Chunks the codes GET_V and
        VAR_D provide relative 'addresses' of where their
        variables should go/are so we update this offset
        everytime we switch Chunks
    */
    size_t varOffset = 0;
    size_t constOffset = 0;

    // instruction pointer
    size_t ip;

    Chunk cur;

    std::vector<CompileConst> constants;
    std::vector<CompileVar> vars;

    Stack stack;

    VM() = default;

    void PrintStack();

    void SetChunk(Chunk &c);

    void Jump(size_t jump);
    void ExecuteCurrentChunk();
    void ExecuteInstruction();
    void StepThrough();
};