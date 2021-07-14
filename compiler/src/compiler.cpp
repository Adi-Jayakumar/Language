#include "compiler.h"

Compiler::Compiler()
{
    Functions.push_back(Function());
    Functions[0].arity = 0;
    cur = &Functions[0];
}

void Compiler::CompileError(Token loc, std::string err)
{
    Error e = Error("[COMPILE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    mainIndex = SIZE_MAX;
    size_t numFunctions = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        s[i]->NodeCompile(*this);
        if (dynamic_cast<FuncDecl *>(s[i].get()) != nullptr)
        {
            numFunctions++;
            FuncDecl *asFD = static_cast<FuncDecl *>(s[i].get());
            if (asFD->argtypes.size() == 0 && asFD->name == "Main")
            {
                if (mainIndex != SIZE_MAX)
                    CompileError(asFD->Loc(), "Main function already defined");
                mainIndex = numFunctions;
            }
        }
        else if (dynamic_cast<DeclaredVar *>(s[i].get()) == nullptr && dynamic_cast<StructDecl *>(s[i].get()) == nullptr && dynamic_cast<ImportStmt *>(s[i].get()) == nullptr)
            CompileError(s[i]->Loc(), "Only declarations allowed in global region");
    }
}

void Compiler::Disassemble()
{
    std::cout << "NUM FunctionS: " << Functions.size() << std::endl
              << std::endl
              << std::endl;
    for (size_t i = 0; i < Functions.size(); i++)
    {
        std::cout << "Function index: " << i << std::endl
                  << "Function arity: " << +Functions[i].arity
                  << std::endl
                  << std::endl;

        Functions[i].PrintCode();

        std::cout << std::endl
                  << std::endl;
    }
}

void Compiler::ClearCurrentDepthWithPOPInst()
{
    while (Symbols.vars.size() > 0 && Symbols.vars.back().depth == Symbols.depth)
    {
        Symbols.vars.pop_back();
        cur->code.push_back({Opcode::POP, 0});
    }
}