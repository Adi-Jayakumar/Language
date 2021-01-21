#include "compiler.h"

Compiler::Compiler()
{
    chunks.push_back(Chunk());
}

void Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    for(auto &stmt : s)
    {
        if(dynamic_cast<FuncDecl*>(stmt.get()) != nullptr)
        {
            Chunk d = Chunk();
            stmt->NodeCompile(d);
            chunks.push_back(d);
        }
        else
            stmt->NodeCompile(chunks[0]);
    }
}

void Compiler::Disassemble()
{
    std::cout << "NUM CHUNKS: " << chunks.size() << std::endl;
    for (size_t i = 0; i < chunks.size(); i++)
    {
        std::cout << "Chunk index: " << i << std::endl;
        chunks[i].PrintCode();
        std::cout << std::endl;
    }
}