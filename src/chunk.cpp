#include "chunk.h"

void Chunk::PrintCode()
{
    for (Op &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::GET_C)
            std::cout << " '" << constants[o.op1] << "' at index: " << +o.op1;
        else if (o.code == Opcode::GET_V || o.code == Opcode::VAR_D || o.code == Opcode::VAR_A)
            std::cout << " '" << vars[o.op2].name << "' at runtime index: " << +o.op1 << " at compile time index: " << o.op2;
        else
            std::cout << " " << +o.op1;

        std::cout << std::endl;
    }
}

size_t Chunk::ResolveVariable(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if ((vars[i].name.length() == name.length()) && (vars[i].name == name))
            return i;
    }
    return 255;
}

void Chunk::CleanUpVariables()
{
    // while (!vars.empty() && vars.back().depth == depth)
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].depth == depth)
        {
            numPops++;
            code.push_back({Opcode::POP, 0, 0});
            code.push_back({Opcode::DEL_V, 0, 0});
        }
    }
}