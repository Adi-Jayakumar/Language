#include "compiler.h"

Compiler::Compiler()
{
    chunks.push_back(Chunk());
    chunks[0].arity = 0;
    cur = &chunks[0];
}

void Compiler::CompileError(std::string err)
{
    Error e = Error("[COMPILE ERROR] " + err);
    e.Dump();
}

size_t Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        s[i]->NodeCompile(*this);
        if (dynamic_cast<FuncDecl *>(s[i].get()))
        {
            FuncDecl *asFD = dynamic_cast<FuncDecl *>(s[i].get());
            if (asFD->ret == 0 && asFD->params.size() == 0 && asFD->name == "Main")
                return i;
        }
    }
    cur->CleanUpVariables();
    return SIZE_MAX;
}

void Compiler::Disassemble()
{
    std::cout << "NUM CHUNKS: " << chunks.size() << std::endl
              << std::endl
              << std::endl;
    for (size_t i = 0; i < chunks.size(); i++)
    {
        std::cout << "Chunk index: " << i << std::endl
                  << "Chunk arity: " << +chunks[i].arity
                  << std::endl
                  << std::endl;

        chunks[i].PrintCode();

        std::cout << std::endl
                  << std::endl;
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