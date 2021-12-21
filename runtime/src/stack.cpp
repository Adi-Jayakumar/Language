#include "stack.h"

Stack::Stack()
{
    data = new char[DEF_SIZE * sizeof(char)];
    top = data;
    size = 0;
    capacity = DEF_SIZE;
}

Stack::~Stack()
{
    delete[] data;
}

void Stack::GrowIfUnableToPush(const size_t n)
{
    if (size + n < capacity)
        return;

    capacity *= 2;
    char *old = data;

    data = new char[capacity];
    std::memcpy(data, old, size);
    delete[] old;
}