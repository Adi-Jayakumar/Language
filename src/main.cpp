#include "ASTPrinter.h"
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

    std::shared_ptr<Stmt> res = p.ParseBlock();
    std::cout << res.get() << std::endl;
    TypeChecker t = TypeChecker();
    t.TypeCheck(res);

    Compiler c = Compiler();
    c.Compile(res);
    c.Disassemble();
}