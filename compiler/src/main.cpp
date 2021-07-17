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
    // // c.Functions[0] = c.Functions[1];
    // // std::ofstream file;
    // // file.open("../test.lo", std::ios::out | std::ios::app | std::ios::binary);

    // // c.Functions[0].SerialiseFunction(file);
    // // c.Functions[1].SerialiseFunction(file);

    // // std::ifstream fil;
    // // fil.open("../test.lo", std::ios::in | std::ios::binary);
    // // Function f = Function::DeserialiseFunction(fil);
    // // f.PrintCode();
    // // Function g = Function::DeserialiseFunction(fil);
    // // g.PrintCode();

    // std::cout << "Struct inheritance tree in main" << std::endl;

    // for (const auto &kv : c.StructTree)
    // {
    //     std::cout << GetTypeStringMap()[kv.first] << "\t|\t";
    //     for (const auto &ch : kv.second)
    //         std::cout << GetTypeStringMap()[ch] << ", ";
    //     std::cout << std::endl;
    // }

    // std::cout << std::endl
    //           << std::endl;

    // Compiler::SerialiseProgram(c, "../test.lo");
}