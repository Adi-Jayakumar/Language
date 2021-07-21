#include "ASTPrinter.h"
#include "compiler.h"
#include "parser.h"
#include "serialise.h"
#include "staticanalyser.h"
#include "argparser.h"
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

int main(int argc, char **argv)
{
    ArgParser arg;

    arg.AddSwitch("-p");
    arg.AddSwitch("-t");
    arg.AddSwitch("-c");

    arg.AddArg({"-f", "-o"});

    arg.ParseArgs(argc - 1, argv + 1);

    std::string ifPath = arg.GetArgVal("-f");

    Parser p(ifPath);
    std::vector<std::shared_ptr<Stmt>> parsed = p.Parse();

    if (arg.IsSwitch("-p"))
    {
        std::cout << "PARSED" << std::endl;
        for (auto &stmt : parsed)
            std::cout << stmt.get() << std::endl;
    }

    StaticAnalyser s;
    for (auto &stmt : parsed)
        stmt->Type(s);

    if (arg.IsSwitch("-t"))
    {
        std::cout << "\n\nANALYSED" << std::endl;
        for (auto &stmt : parsed)
            std::cout << stmt.get() << std::endl;
    }

    Compiler c;
    c.Compile(parsed);

    if (arg.IsSwitch("-c"))
    {
        std::cout << "\n\nCOMPILED" << std::endl;
        c.Disassemble();
    }

    std::string ofPath = arg.GetArgVal("-o");

    Compiler::SerialiseProgram(c, ofPath);
}