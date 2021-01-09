#include "ASTPrinter.h"
#include "parser.h"

void DumpTokens(std::string fPath)
{
    Lexer l = Lexer(fPath);
    Token t = l.NextToken();
    while(t.type != TokenID::END)
    {
        std::cout << t << std::endl;
        t = l.NextToken();
    }
}


int main()
{
    Parser p = Parser("ex/test.txt");

    DumpTokens("ex/test.txt");

    std::vector<Stmt*> res = p.Parse();
    for(Stmt* s : res)
    {
        std::cout << s << std::endl;
    }

    for(Stmt * s : res)
    {
        delete s;
    }

}