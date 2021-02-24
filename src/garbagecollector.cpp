#include "vm.h"

std::string ToString(const GCSate &gcs)
{
    switch (gcs)
    {
    case GCSate::UNMARKED:
    {
        return "UNMARKED";
    }
    case GCSate::MARKED:
    {
        return "MARKED";
    }
    case GCSate::FREED:
    {
        return "FREED";
    }
    default:
    {
        return "I SHOULD NOT BE PRINTED";
    }
    }
}

std::ostream &operator<<(std::ostream &out, const GCSate &gcs)
{
    out << ToString(gcs);
    return out;
}

void GC::MarkObject(RuntimeObject &rto)
{
    rto.state = GCSate::MARKED;

    if (rto.t.isArray)
    {
        for (size_t i = 0; i < rto.as.arr.size; i++)
            MarkObject(rto.as.arr.data[i]);
    }
}

void GC::FreeObject(RuntimeObject &rto)
{
    rto.state = GCSate::FREED;

    if (rto.t.isArray)
    {
        RTArray *arr = &rto.as.arr;

        for (size_t i = 0; i < arr->size; i++)
            FreeObject(arr->data[i]);
    }
    else
    {
        if (rto.t.type == 4)
        {
            RTString *str = &rto.as.str;
            free(str->data);
        }
    }
}

void GC::MarkRoots(VM *vm)
{
    for (size_t i = 0; i < vm->stack.count; i++)
        MarkObject(vm->stack[i]);
}

void GC::FreeUnMarked(VM *vm)
{
    Chunk *cur = &vm->functions[vm->curChunk];

    for (size_t i = 0; i < cur->constants.size(); i++)
    {
        if (cur->constants[i].state == GCSate::UNMARKED)
            FreeObject(cur->constants[i]);
    }
}

void GC::GarbageCollect(VM *vm)
{
    MarkRoots(vm);
    FreeUnMarked(vm);
}