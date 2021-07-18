#pragma once
#include "callstack.h"
#include "function.h"
#include "libfuncdef.h"
#include "perror.h"
#include "runtimeobject.h"
#include "serialise.h"
#include "stack.h"
#include "throwinfo.h"
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

class VM;
typedef Object *(*LibFunc)(VM *, Object **);

class Heap
{
    std::vector<Object *> values;
    size_t threshold;

public:
    Heap() = default;
    size_t Size() { return values.size(); };
    Object *operator[](size_t index) { return values[index]; };
    void AddObject(Object *obj) { values.push_back(obj); };
    void CleanUp(); // does nothing if the number of unmarked objects is not sufficiently large
};

class VM
{
    // private:
public:
    std::vector<Function> functions;
    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;

    std::vector<std::pair<LibFunc, size_t>> CLibs;
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
    Stack stack;
    Heap heap;

    void Jump(size_t jump);
    void ExecuteInstruction();

    /*------------------------------------Garbage collector------------------------------------*/

#define GC_DEBUG_OUTPUT // prints information about which objects are being marked/freed
    // #define GC_STRESS            // does a round of GC whenever new memory is requested
    // deletes any memory owned by the object
    void FreeObject(Object *obj);
    void MarkRoots();
    void FreeUnmarked();
    void ResetObjects();
    void GarbageCollect();

public:
    VM(std::vector<Function> &functions, size_t mainIndex, std::unordered_map<size_t, std::unordered_set<size_t>> &StructTree, std::vector<LibraryFunctionDef> &, std::vector<ThrowInfo> &);
    ~VM();
    void Disasemble();

    size_t GetStackSize()
    {
        return stack.count;
    };
    void PrintStack();
    void ExecuteProgram();

    // adds a list of Create*()'ed objects to the VM's heap
    // must be used in C-Libraries after Create*()'ing objects
    // so that the garbage collector knows about them
    void AddToHeap(Object **objs, size_t numObjs);
    void AddSingleToHeap(Object *obj) { heap.AddObject(obj); };

    void RuntimeError(std::string msg);

    // native functions
    void NativePrint(int arity); // opcode: 0
    void NativeToString();       // opcode: 1

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
    static std::vector<Op> DeserialiseOps(std::ifstream &file);
    static std::vector<ThrowInfo> DeserialiseThrowInfos(std::ifstream &file);
};