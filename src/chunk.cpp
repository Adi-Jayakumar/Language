#include "chunk.h"

void Chunk::PrintCode()
{
    for (Op &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::LOAD_INT)
            std::cout << " at index: " << +o.op << " value: " << ints[o.op];
        else if (o.code == Opcode::LOAD_DOUBLE)
            std::cout << " at index: " << +o.op << " value: " << doubles[o.op];
        else if (o.code == Opcode::LOAD_BOOL)
            std::cout << " at index: " << +o.op << " value: " << (bools[o.op] ? "true" : "false");
        else if (o.code == Opcode::LOAD_STRING)
            std::cout << " at index: " << +o.op << " value: " << strings[o.op];
        else if (o.code == Opcode::LOAD_CHAR)
            std::cout << " at index: " << +o.op << " value: " << chars[o.op];
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