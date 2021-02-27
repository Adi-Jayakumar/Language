#pragma once
#include "callstack.h"
#include "runtimefunc.h"
#include "runtimeobject.h"
#include "stack.h"

struct VM
{
    std::vector<RuntimeFunction> functions;
    std::vector<RuntimeObject> globals;

    // instruction pointer
    size_t ip;

    // Call stack
    CallFrame *cs;

    // current CallFrame
    CallFrame *curCF;

    // current function index
    size_t curFunc;

    std::vector<RuntimeObject> constants;

    Array stack;
    Array RTAllocValues;

    VM(std::vector<RuntimeFunction> &, size_t);
    ~VM();

    void PrintStack();

    RuntimeObject *Allocate(size_t);
    char *StringAllocate(size_t);

    void RuntimeError(std::string msg);
    void Jump(size_t jump);
    void ExecuteProgram();
    void ExecuteInstruction();

    void NativePrint(int arity);
};

// #define GC_DEBUG_OUTPUT
// #define GC_STRESS_TEST
// garbage collector for VM
namespace GC
{
    void MarkObject(RuntimeObject *);
    // frees any memory owned by the RTO pointed to
    void FreeObject(RuntimeObject *);
    // frees any memory owned by the RTO pointed to and memory
    // pointed to
    void DestroyObject(RuntimeObject *);
    void MarkRoots(VM *vm);
    void FreeUnMarked(VM *vm);
    void GarbageCollect(VM *vm);
} // namespace GC