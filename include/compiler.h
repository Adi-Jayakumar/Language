#pragma once
#include "chunk.h"
#include "stmtnode.h"



struct Compiler
{
    std::vector<Chunk> chunks;
    Chunk* cur;
    Compiler();
    void Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();
};