#include "perror.h"

Error::Error(std::string s)
{
    msg = s;
}

void Error::Dump()
{
    std::cout << msg << std::endl;
    exit(3);
}

Error &operator+(Error &out, std::string s)
{
    out.msg = out.msg + s;
    return out;
}
