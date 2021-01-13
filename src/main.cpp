// #include "ASTPrinter.h"
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
    // for (Stmt *s : res->stmts)
    // {
    //     std::cout << s << std::endl;
    //     std::cout << "Resulting type: " << +s->Type() << std::endl;
    // }

    delete res;

    // Expr *e = p.Assignment();
    // std::cout << e << std::endl;
    // Expr * e = new Assign("x", new Literal({TokenID::INT_L, "2", 12}));
}