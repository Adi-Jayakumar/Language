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

void GC::MarkObject(RuntimeObject *rto)
{

#ifdef GC_DEBUG_OUTPUT
    std::cout << "Marking " << *rto << std::endl;
#endif

    rto->state = GCSate::MARKED;

    if (rto->t == RuntimeType::ARRAY)
    {
        for (size_t i = 0; i < rto->as.arr.size; i++)
            MarkObject(&rto->as.arr.data[i]);
    }
}

void GC::FreeObject(RuntimeObject *rto)
{
#ifdef GC_DEBUG_OUTPUT
    std::cout << "Freeing " << *rto << std::endl;
#endif

    rto->state = GCSate::FREED;

    if (rto->t == RuntimeType::ARRAY)
    {
        RTArray *arr = &rto->as.arr;
        for (size_t i = 0; i < arr->size; i++)
            FreeObject(&arr->data[i]);
        free(arr->data);
    }
    else if (rto->t == RuntimeType::STRING)
    {
        RTString *str = &rto->as.str;
        free(str->data);
    }
    else if (rto->t == RuntimeType::STRUCT)
        free(rto->as.strct);
}

void GC::DestroyObject(RuntimeObject *rto)
{
#ifdef GC_DEBUG_OUTPUT
    std::cout << "Destroying " << *rto << std::endl;
#endif
    if (rto->state != GCSate::FREED)
    {
        if (rto->t == RuntimeType::ARRAY)
        {
            RTArray *arr = &rto->as.arr;
            for (size_t i = 0; i < arr->size; i++)
                DestroyObject(&arr->data[i]);
        }
        FreeObject(rto);
    }
    free(rto);
}

void GC::MarkRoots(VM *vm)
{
    for (size_t i = 0; i < vm->stack.count; i++)
        MarkObject(vm->stack[i]);
}

void GC::FreeUnMarked(VM *vm)
{
    RuntimeFunction *cur = &vm->functions[vm->curFunc];

    for (size_t i = 0; i < cur->values.size(); i++)
    {
        if (cur->values[i].state == GCSate::UNMARKED)
            FreeObject(&cur->values[i]);
    }

    for (size_t j = 0; j < vm->RTAllocValues.count; j++)
    {
        if (vm->RTAllocValues[j]->state == GCSate::UNMARKED)
            FreeObject(vm->RTAllocValues[j]);
    }
}

void GC::GarbageCollect(VM *vm)
{
    MarkRoots(vm);
    FreeUnMarked(vm);
}