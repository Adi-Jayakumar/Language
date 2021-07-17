#pragma once
#include "function.h"
#include "idstructs.h"
#include "libfuncdef.h"
#include "perror.h"
#include "serialise.h"
#include "stmtnode.h"
#include "symboltable.h"
#include "throwinfo.h"
#include <fstream>
#include <stack>

class Compiler
{
public:
    size_t mainIndex;
    std::vector<Function> Functions;
    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;

    SymbolTable Symbols;
    std::vector<LibraryFunctionDef> libfuncs;

    std::stack<std::vector<size_t>> breakIndices;
    std::vector<ThrowInfo> throwStack;

    bool hadError = false;

    void CompileError(Token loc, std::string err);

    Function *cur;
    Compiler();

    // sets the index of the 'void Main()' function
    void Compile(std::vector<std::shared_ptr<Stmt>> &s);
    void Disassemble();

    void ClearCurrentDepthWithPOPInst();

    static void SerialiseProgram(Compiler &prog, std::string fPath);

private:
    static void SerialisationError(std::string err);
    static bool DoesFileExist(std::string &path);
    static void SerialiseFunction(Function &f, std::ofstream &file);
    static void SerialiseData(void *data, size_t typeSize, size_t numElements, std::ofstream &file);
    static void SerialiseInts(Function &f, std::ofstream &file);
    static void SerialiseDoubles(Function &f, std::ofstream &file);
    static void SerialiseBools(Function &f, std::ofstream &file);
    static void SerialiseChars(Function &f, std::ofstream &file);
    static void SerialiseStrings(Function &f, std::ofstream &file);
    static void SerialiseOps(Function &f, std::ofstream &file);
};