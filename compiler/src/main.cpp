#include "ASTPrinter.h"
#include "compiler.h"
#include "parser.h"
#include "serialise.h"
#include "staticanalyser.h"
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
    DumpTokens("../verifier_ex/square.txt");
    std::cout << std::endl
              << std::endl;
    Parser p("../verifier_ex/square.txt");
    std::vector<std::shared_ptr<Stmt>> res = p.Parse();

    if (p.hadError)
        exit(2);

    for (auto &s : res)
        std::cout << s.get() << std::endl;

    std::cout << std::endl
              << std::endl;

    StaticAnalyser t = StaticAnalyser();
    for (auto &s : res)
    {
        t.TypeCheck(s);
    }

    // if (t.hadError)
    //     exit(3);

    std::cout << std::endl
              << std::endl;

    for (auto &s : res)
        std::cout << s.get() << std::endl;

    std::cout << std::endl
              << std::endl;

    Compiler c = Compiler();
    c.Compile(res);

    c.Disassemble();

    std::cout << "THROW STACK" << std::endl;
    for (auto &ti : c.throwStack)
    {
        std::cout << "TrowInfos(" << +ti.func << ", " << +ti.index << ", " << ti.isArray << ", " << +ti.type << ", " << ti.callStackIndex << ")" << std::endl;
    }

    Compiler::SerialiseProgram(c, "../test.lo");
}