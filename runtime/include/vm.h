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
    std::unordered_map<oprand_t, std::unordered_set<oprand_t>> struct_tree;

    // std::vector<std::pair<LibFunc, size_t>> CLibs;
    std::vector<void *> lib_handles;

    std::vector<ThrowInfo> throw_infos;
    std::stack<ThrowInfo> throw_stack;

    // instruction pointer
    size_t ip;

    // Call stack
    std::vector<CallFrame> cs;
    CallFrame *cur_cf;

    // current function index
    size_t cur_func;
    size_t cur_routine;
    Stack stack;

    void Jump(size_t jump);
    void ExecuteInstruction();

public:
    VM() = default;
    VM(std::vector<Function> &functions,
       oprand_t main_index,
       std::unordered_map<oprand_t, std::unordered_set<oprand_t>> &struct_tree,
       std::vector<LibraryFunctionDef> &,
       std::vector<ThrowInfo> &);

    void Disasemble();
    void PrintCallStack();
    void ExecuteProgram();

    void RuntimeError(const std::string &msg);
    static VM DeserialiseProgram(const std::string &fPath);

private:
    static bool DoesFileExist(const std::string &path);
    static void DeserialisationError(const std::string &err);
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