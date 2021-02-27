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

void Array::push_back_copy(RuntimeObject rto)
{
    if (count < capacity)
    {
        data[count] = new RuntimeObject(rto);
        back = data[count];
    }
    else
    {
        capacity *= GROW_FAC;
        RuntimeObject **more = (RuntimeObject **)malloc(capacity * sizeof(RuntimeObject *));
        memcpy(more, data, count * sizeof(RuntimeObject *));
        free(data);
        data = more;
        data[count] = new RuntimeObject(rto);
        back = data[count];
    }
    count++;
}

void Array::pop_back()
{
    count--;
    back = count > 0 ? data[count - 1] : data[0];
}

void Array::pop_N(size_t n)
{
    count -= n;
    back = count > 0 ? data[count - 1] : data[0];
}

RuntimeObject *Array::operator[](const size_t index)
{
    return data[index];
}