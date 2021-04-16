#pragma once
#include "chunk.h"
#include "idstructs.h"
#include "perror.h"
#include "stmtnode.h"

struct Compiler
{
    std::vector<Chunk> chunks;
    // std::vector<FuncID> funcs{{"Print", {false, 0}}, {"ToString", {false, 4}}};
    std::vector<StructID> structs;
    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;
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