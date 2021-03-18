#include "vm.h"

void GC::FreeObject(RuntimeObject *rto)
{
#ifdef GC_DEBUG_OUTPUT
    std::cout << "Freeing " << *rto << std::endl;
#endif

    if (rto->state == GCState::FREED)
        return;

    rto->state = GCState::FREED;

    if (rto->t == RuntimeType::ARRAY || rto->t == RuntimeType::STRUCT)
        free(rto->as.arr.data);
    else if (rto->t == RuntimeType::STRING)
        free(rto->as.str.data);

#ifdef GC_DEBUG_OUTPUT
    std::cout << "Finished Freeing " << rto << std::endl;
#endif
}

void GC::DeallocateHeap(VM *vm)
{
    for (size_t i = 0; i < vm->Heap.count; i++)
    {
        FreeObject(vm->Heap[i]);
        free(vm->Heap[i]);
    }
}

void GC::MarkObject(RuntimeObject *rto)
{
    if (rto->state == GCState::MARKED)
        return;

#ifdef GC_DEBUG_OUTPUT
    std::cout << "Started Marking " << *rto << std::endl;
#endif

    rto->state = GCState::MARKED;

    if (rto->t == RuntimeType::ARRAY || rto->t == RuntimeType::STRUCT)
    {
        for (size_t i = 0; i < rto->as.arr.size; i++)
            MarkObject(rto->as.arr.data[i]);
    }

#ifdef GC_DEBUG_OUTPUT
    std::cout << "Finished Marking " << *rto << std::endl;
#endif
}

void GC::MarkRoots(VM *vm)
{
    for (size_t i = 0; i < vm->stack.count; i++)
        MarkObject(vm->stack[i]);
}

void GC::FreeUnmarked(VM *vm)
{
    for (size_t i = 0; i < vm->Heap.count; i++)
    {
        if (vm->Heap[i]->state == GCState::UNMARKED)
            FreeObject(vm->Heap[i]);
    }
}

void GC::ResetObjects(VM *vm)
{
    for (RuntimeFunction &rtf : vm->functions)
    {
        for (RuntimeObject &rto : rtf.values)
        {
            if (rto.state != GCState::FREED)
                rto.state = GCState::UNMARKED;
        }
    }

    for (size_t i = 0; i < vm->Heap.count; i++)
    {
        if (vm->Heap[i]->state != GCState::FREED)
            vm->Heap[i]->state = GCState::UNMARKED;
    }
}

void GC::GarbageCollect(VM *vm)
{
    MarkRoots(vm);
    FreeUnmarked(vm);
    ResetObjects(vm);
}