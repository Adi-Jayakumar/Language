#pragma once
#include "callstack.h"
#include "function.h"
#include "libfuncdef.h"
#include "perror.h"
#include "serialise.h"
#include "stack.h"
#include "throwinfo.h"
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

class VM
{
    // private:
public:
    std::vector<Function> functions;
    std::unordered_map<oprand_t, std::unordered_set<oprand_t>> StructTree;

    // std::vector<std::pair<LibFunc, size_t>> CLibs;
    std::vector<void *> libHandles;

    std::vector<ThrowInfo> throwInfos;
    std::stack<ThrowInfo> ThrowStack;

    // instruction pointer
    size_t ip;

    // Call stack
    std::vector<CallFrame> cs;

    // current CallFrame
    CallFrame *curCF;

    // current function index
    size_t curFunc;
    size_t curRoutine;
    Stack stack;

    void Jump(size_t jump);
    void ExecuteInstruction();

public:
    VM() = default;
    VM(std::vector<Function> &functions,
       oprand_t mainIndex,
       std::unordered_map<oprand_t, std::unordered_set<oprand_t>> &StructTree,
       std::vector<LibraryFunctionDef> &,
       std::vector<ThrowInfo> &);

    void Disasemble();

    size_t GetStackSize()
    {
        return stack.count;
    };
    void PrintCallStack();
    void ExecuteProgram();

    void RuntimeError(std::string msg);

    static VM DeserialiseProgram(std::string fPath);

private:
    static bool DoesFileExist(std::string &path);
    static void DeserialisationError(std::string err);
    static Function DeserialiseFunction(std::ifstream &file);
    static size_t ReadSizeT(std::ifstream &file);
    static void *DeserialiseData(size_t numElements, size_t typeSize, std::ifstream &file);
    static std::vector<int> DeserialiseInts(std::ifstream &file);
    static std::vector<double> DeserialiseDoubles(std::ifstream &file);
    static std::vector<bool> DeserialiseBools(std::ifstream &file);
    static std::vector<char> DeserialiseChars(std::ifstream &file);
    static std::vector<std::string> DeserialiseStrings(std::ifstream &file);
    static std::vector<std::vector<Op>> DeserialiseOps(std::ifstream &file);
    static std::vector<ThrowInfo> DeserialiseThrowInfos(std::ifstream &file);
};