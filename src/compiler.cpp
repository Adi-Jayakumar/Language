#include "compiler.h"

Compiler::Compiler()
{
    chunks.push_back(Chunk());
    chunks[0].arity = 0;
    cur = &chunks[0];
    isFunc = false;
}

void Compiler::CompileError(Token loc, std::string err)
{
    Error e = Error("[COMPILE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err);
    e.Dump();
}

size_t Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    size_t mainIndex = SIZE_MAX;
    size_t numFunctions = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        s[i]->NodeCompile(*this);
        if (dynamic_cast<FuncDecl *>(s[i].get()) != nullptr)
        {
            numFunctions++;
            FuncDecl *asFD = static_cast<FuncDecl *>(s[i].get());
            if (asFD->ret == 0 && asFD->params.size() == 0 && asFD->name == "Main")
                mainIndex = numFunctions;
        }
        else if (dynamic_cast<DeclaredVar *>(s[i].get()) == nullptr)
            CompileError(s[i]->Loc(), "Only declarations allowed in global region");
    }
    cur->CleanUpVariables();
    return mainIndex;
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

bool Compiler::ResolveVariable(std::string &name, size_t &index)
{
    index = cur->ChunkResolveVariable(name);
    if (index == SIZE_MAX)
    {
        index = chunks[0].ChunkResolveVariable(name);
        return true;
    }
    return false;
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