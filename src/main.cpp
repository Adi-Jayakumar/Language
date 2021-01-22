#include "ASTPrinter.h"
#include "parser.h"
#include "typechecker.h"
#include "vm.h"
#include <cstring>

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

    Parser p = Parser("ex/func.txt");

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

    VM vm = VM(c.chunks);
    vm.ExecuteCurrentChunk();

    std::cout << std::endl
              << std::endl;

    std::cout << "Vars not cleaned up:" << std::endl;
    for (CompileVar &cv : vm.vars)
    {
        std::cout << cv << std::endl;
    }

    std::cout << std::endl
              << std::endl;

    std::cout << "Constants on the stack not cleaned up" << std::endl;
    while (!vm.stack.Empty())
    {
        std::cout << vm.stack.Top() << std::endl;
        vm.stack.Pop();
    }
}