#include "vm.h"

void GC::FreeObject(RuntimeObject *rto)
{
    if (rto->state == GCState::FREED)
        return;

    rto->state = GCState::FREED;

    if (rto->t == RuntimeType::ARRAY || rto->t == RuntimeType::STRUCT)
        free(rto->as.arr.data);
    else if (rto->t == RuntimeType::STRING)
        free(rto->as.str.data);
}

void GC::DeallocateHeap(VM *vm)
{
    for (size_t i = 0; i < vm->Heap.count; i++)
    {
        FreeObject(vm->Heap[i]);
        free(vm->Heap[i]);
    }
}