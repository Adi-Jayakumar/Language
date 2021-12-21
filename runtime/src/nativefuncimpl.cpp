#include "nativefuncimpl.h"

bool operator==(const ReturnValue &lhs, const ReturnValue &rhs)
{
    return lhs.data == rhs.data && lhs.size == rhs.size;
}

bool operator!=(const ReturnValue &lhs, const ReturnValue &rhs)
{
    return !operator==(lhs, rhs);
}

ReturnValue PrintInt(char *x)
{
    std::cout << *(int *)x << std::endl;
    return NULL_RETURN;
}

ReturnValue PrintDouble(char *x)
{
    std::cout << *(double *)x << std::endl;
    return NULL_RETURN;
}

ReturnValue PrintBool(char *x)
{
    const char *res = *(bool *)x ? "true" : "false";
    std::cout << res << std::endl;
    return NULL_RETURN;
}

ReturnValue PrintString(char *x)
{
    int len = *(int *)(x + PTR_SIZE);
    std::cout << std::string(*(char **)x, len) << std::endl;
    return NULL_RETURN;
}

ReturnValue PrintChar(char *x)
{
    std::cout << *x << std::endl;
    return NULL_RETURN;
}
