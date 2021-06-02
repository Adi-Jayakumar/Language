#include "stack.h"

Stack::Stack()
{
    data = (RuntimeObject **)malloc(DEF_SIZE * sizeof(RuntimeObject *));
    count = 0;
    capacity = DEF_SIZE;
    back = *data;
}

Stack::~Stack()
{
    free(data);
}

void Stack::push_back(RuntimeObject *cc)
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

void Stack::push_back_copy(RuntimeObject *rto)
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

void Stack::pop_back()
{
    SetGCState(back, GCState::UNMARKED);
    count--;
    back = count > 0 ? data[count - 1] : data[0];
}

void Stack::pop_N(size_t n)
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

RuntimeObject *Stack::operator[](const size_t index)
{
    return data[index];
}