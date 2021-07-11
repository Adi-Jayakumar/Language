#pragma once
#include "function.h"
#include "idstructs.h"
#include "libfuncdef.h"
#include "perror.h"
#include "stmtnode.h"
#include "symboltable.h"

class Compiler
{
public:
    std::vector<Function> Functions;
    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;

    SymbolTable Symbols;
    std::vector<LibraryFunctionDef> libfuncs;

    bool hadError = false;

    void CompileError(Token loc, std::string err);

    Function *cur;
    Compiler();

    // returns the index of the 'void Main()' function
    size_t Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();

    void ClearCurrentDepthWithPOPInst();
};