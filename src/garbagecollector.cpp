#include "vm.h"

void GC::FreeObject(RuntimeObject *rto)
{
    if (rto->state == GCState::FREED)
        return;

    rto->state = GCState::FREED;

    if (rto->t == RuntimeType::ARRAY || rto->t == RuntimeType::STRUCT)
        rto->as.arr.size > 1 ? delete[] rto->as.arr.data : delete rto->as.arr.data;
    else if (rto->t == RuntimeType::STRING)
        rto->as.str.len > 1 ? delete[] rto->as.str.data : delete rto->as.str.data;
}