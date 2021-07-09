#include "ASTPrinter.h"
#include "compiler.h"
#include "parser.h"
#include "returnchecker.h"
#include "staticanalyser.h"
#include "vm.h"
#include <chrono>

void DumpTokens(std::string fPath)
{
    Lexer l = Lexer(fPath);
    Token t = l.NextToken();
    while (t.type != TokenID::END)
    {
        std::cout << t << std::endl;
        t = l.NextToken();
    }
    std::cout << t << std::endl;
}

int main()
{
    DumpTokens("verifier_ex/square.txt");
    std::cout << std::endl
              << std::endl;
    Parser p("verifier_ex/square.txt");
    std::vector<std::shared_ptr<Stmt>> res = p.Parse();

    if (p.hadError)
        exit(2);

    for (auto &s : res)
        std::cout << s.get() << std::endl;

    std::cout << std::endl
              << std::endl;

    // TypeChecker t = TypeChecker();
    // for (auto &s : res)
    // {
    //     t.TypeCheck(s);
    // }

    // // if (t.hadError)
    // //     exit(3);

    // std::cout << std::endl
    //           << std::endl;

    // ReturnChecker rc;
    // for (auto &s : res)
    // {
    //     FuncDecl *asFunc = dynamic_cast<FuncDecl *>(s.get());
    //     TypeData voidType = {0, false};
    //     if (asFunc && asFunc->ret != voidType)
    //     {
    //         asFunc->DoesReturn(asFunc->ret, rc);
    //     }
    // }

    // if (rc.hadError)
    //     exit(4);

    // for (auto &s : res)
    //     std::cout << s.get() << std::endl;

    // std::cout << std::endl
    //           << std::endl;

    // Compiler c = Compiler();
    // size_t mainIndex = c.Compile(res);
    // c.Disassemble();

    // if (c.hadError)
    //     exit(5);

    // std::cout << std::endl
    //           << std::endl;

    // std::cout << "Struct inheritance tree" << std::endl;

    // for (const auto &kv : c.StructTree)
    // {
    //     std::cout << GetTypeStringMap()[kv.first] << "\t|\t";
    //     for (const auto &ch : kv.second)
    //         std::cout << GetTypeStringMap()[ch] << ", ";
    //     std::cout << std::endl;
    // }

    // std::cout << std::endl
    //           << std::endl;

    // std::vector<RuntimeFunction> rf;

    // for (const auto &ch : c.chunks)
    //     rf.push_back(RuntimeFunction(ch));

    // VM vm = VM(rf, mainIndex, c.StructTree, c.libfuncs);

    // auto t1 = std::chrono::high_resolution_clock::now();

    // vm.ExecuteProgram();

    // auto t2 = std::chrono::high_resolution_clock::now();

    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    // std::cout << "Time taken (s): " << (double)duration / 1e6 << std::endl;

    // std::cout << std::endl
    //           << std::endl;

    // std::cout << "Size of stack: " << vm.GetStackSize() << std::endl;

    // if (vm.GetStackSize() != 0)
    //     vm.PrintStack();
}