#pragma once
#include "nodecompiler.h"


struct Compiler
{
    std::vector<Chunk> chunks;
    Compiler();
    void Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();
};