#include "callstack.h"

void CallStack::Push(CallFrame cf)
{
    if (s.size() == UINT8_MAX)
    {
        std::cout << "Stack overflow." << std::endl;
        exit(3);
    }
    s.push_back(cf);
}

CallFrame CallStack::Top()
{
    return s[s.size() - 1];
}

void CallStack::Pop()
{
    s.pop_back();
}

bool CallStack::Empty()
{
    return s.empty();
}

size_t CallStack::Size()
{
    return s.size();
}

CallFrame CallStack::operator[](size_t i)
{
    return s[i];
}
