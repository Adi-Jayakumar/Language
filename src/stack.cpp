#include "stack.h"

Array::Array()
{
    data = (CompileConst *)malloc(DEF_SIZE * sizeof(CompileConst));
    count = 0;
    capacity = DEF_SIZE;
    back = data;
}

Array::~Array()
{
    free(data);
}

void Array::push_back(CompileConst cc)
{
    if (count < capacity)
    {
        data[count] = cc;
        back = &data[count];
    }
    else
    {
        CompileConst *more = (CompileConst *)malloc(GROW_FAC * capacity * sizeof(CompileConst));
        capacity *= GROW_FAC;
        memcpy(more, data, count * sizeof(CompileConst));
        free(data);
        data = more;
        data[count] = cc;
        back = &data[count];

    }
    count++;
}

void Array::pop_back()
{
    count--;
    back = &data[count - 1];
}

CompileConst &Array::operator[](const size_t index)
{
    return data[index];
}