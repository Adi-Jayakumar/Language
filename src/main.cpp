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

    DumpTokens("ex/test.txt");

    std::vector<Stmt *> res = p.Parse();
    for (Stmt *s : res)
    {
        std::cout << s << std::endl;
        if (dynamic_cast<ExprStmt *>(s) != nullptr)
        {
            ExprStmt *es = dynamic_cast<ExprStmt *>(s);
            if(dynamic_cast<Assign*>(es) != nullptr)
                std::cout << "I AM HERE" << std::endl;
        }
        std::cout << "Resulting type: " << +s->Type() << std::endl;
    }

    for (Stmt *s : res)
    {
        delete s;
    }

    // Expr *e = p.Assignment();
    // std::cout << e << std::endl;
    // Expr * e = new Assign("x", new Literal({TokenID::INT_L, "2", 12}));
}