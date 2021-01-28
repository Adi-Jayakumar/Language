#include "ASTPrinter.h"
#include "parser.h"
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
    // DumpTokens("ex/func.txt");

    // std::cout << std::endl
    //           << std::endl;

    // Parser p = Parser("ex/func.txt");
    Parser p = Parser("ex/if.txt");

    std::vector<std::shared_ptr<Stmt>> res = p.Parse();

    std::cout << std::endl
    << std::endl;

    for (auto &s : res)
        std::cout << s.get() << std::endl;

    std::cout << std::endl
    << std::endl;

    TypeChecker t = TypeChecker();
    for (auto &s : res)
        t.TypeCheck(s);

    Compiler c = Compiler();
    c.Compile(res);
    c.Disassemble();

    std::cout << std::endl
    << std::endl;

    // VM vm = VM(c.chunks);

    // auto t1 = std::chrono::high_resolution_clock::now();

    // vm.ExecuteCurrentChunk();

    // auto t2 = std::chrono::high_resolution_clock::now();

    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    // std::cout << "Time taken (s): " << (double)duration / 1e6 << std::endl;

    std::cout << std::endl
    << std::endl;
}