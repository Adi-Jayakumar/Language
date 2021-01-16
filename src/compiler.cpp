#include "compiler.h"

void Compiler::Compile(std::shared_ptr<Stmt> &s)
{
    s->NodeCompile(c);
}

void Compiler::Disassemble()
{
    c.PrintCode();
}