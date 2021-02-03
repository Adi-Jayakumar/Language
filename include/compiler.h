#pragma once
#include "chunk.h"
#include "perror.h"
#include "stmtnode.h"

struct CTFunc
{
    std::string name;
    TypeID ret;
};

struct Compiler
{
    std::vector<Chunk> chunks;
    std::vector<CTFunc> funcs;

    // bool isInFunc = false;
    // size_t curArity = -1;

    void CompileError(Token loc, std::string err);

    Chunk *cur;
    Compiler();

    // returns the index of the 'void Main()' function
    size_t Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();

    size_t ResolveVariableInCur(std::string &name);
    bool ReolveVariable(std::string &name, size_t &index);
    size_t ResolveFunction(std::string &name);
};