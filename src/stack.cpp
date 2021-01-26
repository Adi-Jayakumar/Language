#include "stack.h"

Stack::Stack(size_t preAlloc)
{
    s.reserve(preAlloc);
}

void Stack::Push(CompileConst cc)
{
    s.push_back(cc);
}

CompileConst Stack::Top()
{
    return s[s.size() - 1];
}

void Stack::Pop()
{
    s.pop_back();
}

bool Stack::Empty()
{
    return s.empty();
}

size_t Stack::Size()
{
    return s.size();
}

CompileConst &Stack::operator[](size_t i)
{
    return s[i];
}