#include "stack.h"

Stack::Stack()
{
    data = new Object *[DEF_SIZE * sizeof(Object *)];
    count = 0;
    capacity = DEF_SIZE;
    back = *data;
}

Stack::~Stack()
{
    delete[] data;
}

void Stack::push_back(Object *cc)
{
    if (count < capacity)
    {
        data[count] = cc;
        back = data[count];
    }
    else
    {
        capacity *= GROW_FAC;
        Object **more = new Object *[capacity * sizeof(Object *)];
        memcpy(more, data, count * sizeof(Object *));
        delete[] data;
        data = more;
        data[count] = cc;
        back = data[count];
    }
    count++;
}

void Stack::pop_back()
{
    count--;
    back = count > 0 ? data[count - 1] : data[0];
}

void Stack::pop_N(size_t n)
{
    count -= n;
    back = count > 0 ? data[count - 1] : data[0];
}

Object *Stack::operator[](const size_t index)
{
    return data[index];
}