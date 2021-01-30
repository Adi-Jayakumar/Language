#include "compiler.h"

Compiler::Compiler()
{
    chunks.push_back(Chunk());
    cur = &chunks[0];
}

void Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    for (auto &stmt : s)
    {
        stmt->NodeCompile(*this);
    }
    cur->CleanUpVariables();
}

void Compiler::Disassemble()
{
    std::cout << "NUM CHUNKS: " << chunks.size() << std::endl
              << std::endl
              << std::endl;
    for (size_t i = 0; i < chunks.size(); i++)
    {
        std::cout << "Chunk index: " << i << std::endl
                  << std::endl;

        chunks[i].PrintCode();

        std::cout << std::endl
                  << std::endl;

        std::cout << "Chunk constants" << std::endl;
        for (size_t j = 0; j < chunks[i].constants.size(); j++)
        {
            std::cout << chunks[i].constants[j] << std::endl;
        }
    }
}

size_t Compiler::ResolveVariableInCur(std::string &name)
{
    return cur->ChunkResolveVariable(name);
}

size_t Compiler::ResolveFunction(std::string &name)
{
    for (size_t i = 0; i < funcs.size(); i++)
    {
        if (funcs[i].name == name)
            return i;
    }
    return SIZE_MAX;
}