#pragma once
#include "nodecompiler.h"


struct Compiler
{
    Chunk c;
    Compiler() = default;
    void Compile(std::shared_ptr<Stmt> &s);
    void Disassemble();
};