#pragma once
#include "chunk.h"
#include "stmtnode.h"



struct Compiler
{
    std::vector<Chunk> chunks;
    std::vector<std::string> funcs;

    // bool isInFunc = false;
    // size_t curArity = -1;

    Chunk* cur;
    Compiler();

    void Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();

    size_t ResolveVariableInCur(std::string &name);
    size_t ResolveFunction(std::string &name);
};