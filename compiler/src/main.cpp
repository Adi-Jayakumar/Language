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

int main(int argc, char **argv)
{
    std::string inFilePath(argv[1], strlen(argv[1]));
    std::string outFilePath(argv[2], strlen(argv[2]));

    Parser p(inFilePath);
    std::vector<std::shared_ptr<Stmt>> parsed = p.Parse();

    if (argc == 4)
    {
        std::string printParse(argv[3], 2);
        if (printParse == "-p")
        {
            for (auto &stmt : parsed)
                std::cout << stmt.get() << std::endl;
        }
    }

    StaticAnalyser s;

    for (auto &stmt : parsed)
        stmt->Type(s);

    if (argc == 4)
    {
        std::string printParse(argv[3], 2);
        if (printParse == "-t")
        {
            for (auto &stmt : parsed)
                std::cout << stmt.get() << std::endl;
        }
    }
    else if (argc == 5)
    {
        std::string printParse(argv[4], 2);
        if (printParse == "-t")
        {
            for (auto &stmt : parsed)
                std::cout << stmt.get() << std::endl;
        }
    }

    Compiler c;
    c.Compile(parsed);

    if (argc == 4)
    {
        std::string printParse(argv[3], 2);
        if (printParse == "-c")
            c.Disassemble();
    }
    else if (argc == 5)
    {
        std::string printParse(argv[4], 2);
        if (printParse == "-c")
            c.Disassemble();
    }
    else if (argc == 6)
    {
        std::string printParse(argv[4], 2);
        if (printParse == "-c")
            c.Disassemble();
    }

    Compiler::SerialiseProgram(c, outFilePath);
}