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
    Array Heap;

    VM(std::vector<RuntimeFunction> &, size_t);
    ~VM();

    void PrintStack();
    void PrintValues();

    RuntimeObject *Allocate(size_t);
    char *StringAllocate(size_t);

    void RuntimeError(std::string msg);
    void Jump(size_t jump);
    void ExecuteProgram();
    void ExecuteInstruction();

    // native functions
    void NativePrint(int arity); // opcode: 0
    void NativeToString(int);    // opcode: 1
};

#define GC_DEBUG_OUTPUT
namespace GC
{
    // deletes any memory owned by the object
    void FreeObject(RuntimeObject *rto);
    void DeallocateHeap(VM *vm);
    void MarkObject(RuntimeObject *rto);
}