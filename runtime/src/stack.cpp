#include "stack.h"

Stack::Stack()
{
    data = new int *[DEF_SIZE * sizeof(int *)];
    count = 0;
    capacity = DEF_SIZE;
    back = *data;
}

Stack::~Stack()
{
    delete[] data;
}

void Stack::push_back(int *cc)
{
    if (count < capacity)
    {
        data[count] = cc;
        back = data[count];
    }
    else
    {
        capacity *= GROW_FAC;
        int **more = new int *[capacity * sizeof(int *)];
        memcpy(more, data, count * sizeof(int *));
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

int *Stack::operator[](const size_t index)
{
    return data[index];
}