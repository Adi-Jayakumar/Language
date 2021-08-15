#include "compiler.h"

void Compile(std::vector<std::shared_ptr<Stmt>> &program, std::string out)
{
    Compiler c;
    c.Compile(program);
    Compiler::SerialiseProgram(c, out);
}