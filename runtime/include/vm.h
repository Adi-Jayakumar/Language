#pragma once
#include "callstack.h"
#include "libfuncdef.h"
#include "runtimefunc.h"
#include "runtimeobject.h"
#include "stack.h"
#include <dlfcn.h>
#include <iostream>
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
    std::vector<RuntimeFunction> functions;
    std::vector<Object *> globals;
    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;

    std::vector<std::pair<LibFunc, size_t>> CLibs;
    std::vector<void *> libHandles;

    // instruction pointer
    size_t ip;

    // Call stack
    CallFrame *cs;

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
    VM(std::vector<RuntimeFunction> &functions, size_t mainIndex, std::unordered_map<size_t, std::unordered_set<size_t>> &StructTree, std::vector<LibraryFunctionDef> &);
    ~VM();

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
};