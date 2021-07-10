#pragma once
#include "chunk.h"
#include "idstructs.h"
#include "perror.h"
#include "stmtnode.h"
#include "symboltable.h"
#include "libfuncdef.h"

class Compiler
{
public:
    std::vector<Chunk> chunks;
    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;

    SymbolTable Symbols;
    std::vector<LibraryFunctionDef> libfuncs;

    bool hadError = false;

    void CompileError(Token loc, std::string err);

    Chunk *cur;
    Compiler();

    // returns the index of the 'void Main()' function
    size_t Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();

    void ClearCurrentDepthWithPOPInst();
};