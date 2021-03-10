#pragma once

struct CallFrame
{
    size_t retIndex;
    size_t retChunk;
    size_t valStackMin;
};