#include "ASTPrinter.h"
#include "parser.h"
#include "returnchecker.h"
#include "typechecker.h"
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
}

int main()
{
    // DumpTokens("ex/test.txt");

    std::cout << std::endl
              << std::endl;

    Parser p = Parser("ex/test.txt");

    std::vector<std::shared_ptr<Stmt>> res = p.Parse();

    if (p.hadError)
        exit(2);

    // for (auto &s : res)
    //     std::cout << s.get() << std::endl;

    // std::cout << std::endl
    //           << std::endl;

    TypeChecker t = TypeChecker();
    for (auto &s : res)
    {
        t.TypeCheck(s);
    }

    if (t.hadError)
        exit(3);

    std::cout << std::endl
              << std::endl;

    ReturnChecker rc;
    for (auto &s : res)
    {
        FuncDecl *asFunc = dynamic_cast<FuncDecl *>(s.get());
        TypeData voidType = {false, 0};
        if (asFunc && asFunc->ret != voidType)
        {
            asFunc->DoesReturn(asFunc->ret, rc);
        }
    }

    if (rc.hadError)
        exit(4);

    for (auto &s : res)
        std::cout << s.get() << std::endl;

    std::cout << std::endl
              << std::endl;

    Compiler c = Compiler();
    size_t mainIndex = c.Compile(res);
    c.Disassemble();

    if (c.hadError)
        exit(5);

    std::cout << std::endl
              << std::endl;

    VM vm = VM(c.chunks, mainIndex);

    auto t1 = std::chrono::high_resolution_clock::now();

    vm.ExecuteCurrentChunk();

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Time taken (s): " << (double)duration / 1e6 << std::endl;

    std::cout << std::endl
              << std::endl;

    std::cout << "Size of stack: " << vm.stack.count << std::endl;

    if (vm.stack.count != 0)
    {
        for (size_t i = 0; i < vm.stack.count; i++)
        {
            std::cout << vm.stack.data[i] << std::endl;
        }
    }
}