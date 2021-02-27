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

    // current Chunk index
    size_t curChunk;

    std::vector<RuntimeObject> constants;

    Array stack;

    VM(std::vector<RuntimeFunction> &, size_t);
    ~VM();

    void PrintStack();

    void SetChunk(size_t);

    RuntimeObject *Allocate(size_t);
    char *StringAllocate(size_t);

    void RuntimeError(std::string msg);
    void Jump(size_t jump);
    void ExecuteProgram();
    void ExecuteInstruction();

    void NativePrint(int arity);
};

// garbage collector for VM
namespace GC
{
    void MarkObject(RuntimeObject &);
    void FreeObject(RuntimeObject &);
    void MarkRoots(VM *vm);
    void FreeUnMarked(VM *vm);
    void GarbageCollect(VM *vm);
} // namespace GC