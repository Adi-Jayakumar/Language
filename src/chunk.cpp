#include "chunk.h"

void Chunk::PrintCode()
{
    for (Op &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::GET_C)
            std::cout << " '" << constants[o.op] << "' at index: " << +o.op;
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

size_t Chunk::ChunkResolveVariable(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if ((vars[i].name.length() == name.length()) && (vars[i].name == name))
            return i;
    }
    return SIZE_MAX;
}

void Chunk::CleanUpVariables()
{
    // while (!vars.empty() && vars.back().depth == depth)
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].depth == depth)
        {
            code.push_back({Opcode::POP, 0});
            vars.pop_back();
        }
    }
}