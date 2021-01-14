// #include "ASTPrinter.h"
#include "compiler.h"
#include "parser.h"
#include "typechecker.h"

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
    Parser p = Parser("ex/test.txt");

    // DumpTokens("ex/test.txt");

    Block *res = p.ParseBlock();
    std::cout << res << std::endl;
    res->Type();

    Chunk c = Chunk();
    res->NodeCompile(c);
    c.PrintCode();
    delete res;
}