#pragma once
#include "compiler.h"
#include <stack>
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
    Chunk cur;

    std::vector<CompileConst> constants;
    std::vector<CompileVar> vars;

    std::stack<CompileConst> stack;

    VM() = default;

    void PrintStack();

    void SetChunk(Chunk &c);
    void ExecuteCurrentChunk();
};