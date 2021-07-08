#pragma once
#include "callstack.h"
#include "runtimefunc.h"
#include "runtimeobject.h"
#include "stack.h"
#include <dlfcn.h>

class VM;
typedef Object *(*LibFunc)(VM *, Object **);

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

    std::vector<Object *> constants;

    Stack stack;
    std::vector<Object *> Heap;

    void Jump(size_t jump);
    void ExecuteInstruction();

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

    void RuntimeError(std::string msg);

    // native functions
    void NativePrint(int arity); // opcode: 0
    void NativeToString();       // opcode: 1
};

// #define GC_DEBUG_OUTPUT      // prints information about which objects are being marked/freed
// #define GC_STRESS            // does a round of GC whenever new memory is requested
namespace GC
{
    // deletes any memory owned by the object
    void FreeObject(Object *obj);
    void DeallocateHeap(VM *vm);
    void MarkObject(Object *obj);
    void MarkRoots(VM *vm);
    void FreeUnmarked(VM *vm);
    void ResetObjects(VM *vm);
    void GarbageCollect(VM *vm);
}