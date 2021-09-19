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
    oprand_t mainIndex;
    oprand_t parseIndex;
    std::vector<Function> Functions;
    std::unordered_map<TypeID, std::unordered_set<TypeID>> StructTree;

    std::vector<LibraryFunctionDef> libfuncs;

    std::stack<std::vector<size_t>> breakIndices;
    std::vector<ThrowInfo> throwStack;

    SymbolTable Symbols;

    bool hadError = false;

    void CompileError(Token loc, std::string err);
    void TypeError(Token loc, std::string err);
    void SymbolError(Token loc, std::string err);

    Function *cur;
    std::pair<std::vector<Op> *, size_t> curRoutine;
    bool updateBP = true;
    Compiler();

    void AddCode(Op o);
    size_t CodeSize();
    // returns the index of the routine and the
    // index within the current routine of the
    // last code added
    std::pair<size_t, size_t> LastAddedCodeLoc();
    // sets the oprand at the specified location
    // with the specified value
    void ModifyOprandAt(std::pair<size_t, size_t> loc, oprand_t oprand);
    void ModifyOpcodeAt(std::pair<size_t, size_t> loc, Opcode Opcode);

    void AddRoutine();
    size_t GetCurRoutineIndex();

    void AddFunction();
    // returns the relative location on the stack
    // of the specified variable
    size_t GetVariableStackLoc(std::string &name);

    // sets the index of the 'void Main()' function
    void Compile(std::vector<SP<Stmt>> &s);
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
    static void SerialiseThrowInfo(std::vector<ThrowInfo> &infos, std::ofstream &file);
};