#include "stack.h"

Array::Array()
{
    data = (RuntimeObject **)malloc(DEF_SIZE * sizeof(RuntimeObject *));
    count = 0;
    capacity = DEF_SIZE;
    back = *data;
}

Array::~Array()
{
    free(data);
}

void Array::push_back(RuntimeObject *cc)
{
    if (count < capacity)
    {
        data[count] = cc;
        back = data[count];
    }
    else
    {
        capacity *= GROW_FAC;
        RuntimeObject **more = (RuntimeObject **)malloc(capacity * sizeof(RuntimeObject *));
        memcpy(more, data, count * sizeof(RuntimeObject *));
        free(data);
        data = more;
        data[count] = cc;
        back = data[count];
    }
    count++;
}

void Array::push_back_copy(RuntimeObject *rto)
{
    if (count < capacity)
    {
        data[count] = CopyRTO(rto);
        back = data[count];
    }
    else
    {
        capacity *= GROW_FAC;
        RuntimeObject **more = (RuntimeObject **)malloc(capacity * sizeof(RuntimeObject *));
        memcpy(more, data, count * sizeof(RuntimeObject *));
        free(data);
        data = more;
        data[count] = CopyRTO(rto);
        back = data[count];
    }
    count++;
}

void Array::pop_back()
{
    SetGCState(back, GCState::UNMARKED);
    count--;
    back = count > 0 ? data[count - 1] : data[0];
}

void Array::pop_N(size_t n)
{
    for (size_t i = count - 1; i >= count - n; i--)
    {
        SetGCState(data[i], GCState::UNMARKED);
        if (i == 0)
            break;
    }

    count -= n;
    back = count > 0 ? data[count - 1] : data[0];
}

RuntimeObject *Array::operator[](const size_t index)
{
    return data[index];
}