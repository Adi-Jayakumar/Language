#pragma once
#include "chunk.h"
#include "perror.h"
#include "stmtnode.h"

struct CTFunc
{
    std::string name;
    TypeData ret;
};

struct CTStruct
{
    std::vector<std::string> members;
    std::vector<std::shared_ptr<Expr>> init;
    TypeData type;
    bool isNull;
};

struct Compiler
{
    std::vector<Chunk> chunks;
    std::vector<CTFunc> funcs{{"Print", {false, 0}}, {"ToString", {false, 4}}};
    std::vector<CTStruct> structs;
    bool hadError = false;

    void CompileError(Token loc, std::string err);

    bool isFunc;
    Chunk *cur;
    Compiler();

    // returns the index of the 'void Main()' function
    size_t Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();

    // resolves variables in the current chunk and global variables
    // returns true if it is a global variable
    bool ResolveVariable(std::string &name, size_t &index);
    size_t ResolveVariableInCur(std::string &name);
    size_t ResolveFunction(std::string &name, bool &isNative);
    size_t ResolveStruct(const TypeData &type);
};