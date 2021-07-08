#include "vm.h"

void VM::MarkRoots()
{
    for (size_t i = 0; i < stack.count; i++)
    {
        stack[i]->state = GCState::MARKED;
        stack[i]->MarkChildren();
    }
}

void VM::FreeUnmarked()
{
    for (size_t i = 0; i < heap.Size(); i++)
    {
        if (heap[i]->state != GCState::MARKED && heap[i]->state != GCState::FREED)
        {
            heap[i]->DestroyOwnedMemory();
            heap[i]->state = GCState::FREED;
        }
    }
}

void VM::ResetObjects()
{
    for (size_t i = 0; i < GetStackSize(); i++)
    {
        if (stack[i]->state != GCState::FREED)
            stack[i]->state = GCState::UNMARKED;
    }
}

void VM::GarbageCollect()
{
    MarkRoots();
    FreeUnmarked();
    ResetObjects();
}