#include "chunk.h"

void Chunk::PrintCode()
{
    for (Op &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::GET_C)
        {
            std::cout << " '" << values[o.op] << "' at index: " << +o.op;
        }
        else if (o.code == Opcode::GET_V || o.code == Opcode::VAR_A)
        {
            std::cout << " at relative stack index: " << +o.op;
        }
        else if (o.code == Opcode::CALL_F)
            std::cout << " " << +o.op;
        else
            std::cout << " " << +o.op;

        std::cout << std::endl;
    }
}