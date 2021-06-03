#include "libinfo.h"

extern "C"
{
    void FreeLibInfo(LibInfo li)
    {
        for (size_t i = 0; i < li.size; i++)
        {
            free(li.info[i]);
        }
        free(li.info);
    }
}