#include <cstddef>
#include <cstdlib>

extern "C"
{
    struct LibInfo
    {
        size_t size;
        char **info;
    };

    void FreeLibInfo(LibInfo);
}