#include "vm.h"

void VM::SetChunk(Chunk &_cur)
{
    varOffset = vars.size();
    cur = _cur;
}

void VM::ExecuteCurrentChunk()
{
}