#include "ASTPrinter.h"
#include "argparser.h"
#include "compiler.h"
#include "constantpropagator.h"
#include "constevaluator.h"
#include "parser.h"
#include "serialise.h"
#include "staticanalyser.h"
#include "verifier.h"

void DumpTokens(std::string fPath)
{
    // Lexer l = Lexer(fPath);
    Parser p(fPath);

    while (p.cur.type != TokenID::END)
    {
        std::cout << p.cur << std::endl;
        p.Advance();
    }
    std::cout << p.cur << std::endl;
}

#ifdef COMPILE_FOR_TEST
#define main not_main
#endif

int main(int argc, char **argv)
{

    // DumpTokens("../verifier_ex/square.txt");
    ArgParser arg;

    arg.AddSwitch("-p");
    arg.AddSwitch("-t");
    arg.AddSwitch("-O");
    arg.AddSwitch("-c");
    arg.AddSwitch("--rm-bin");

    arg.AddArg({"-f", "-o"});

    arg.ParseArgs(argc - 1, argv + 1);

    std::string ifPath = arg.GetArgVal("-f");

    Parser p(ifPath);
    std::vector<std::shared_ptr<Stmt>> parsed = p.Parse();

    if (arg.IsSwitchOn("-p"))
    {
        std::cout << "PARSED" << std::endl;
        ASTPrinter ast(false);

        for (auto &stmt : parsed)
            stmt->Print(ast);
        ast.Flush();
    }

    StaticAnalyser s;
    for (auto &stmt : parsed)
        stmt->Type(s);

    if (arg.IsSwitchOn("-t"))
    {
        std::cout << "\n\nANALYSED" << std::endl;
        ASTPrinter ast(true);
        for (auto &stmt : parsed)
            stmt->Print(ast);
        ast.Flush();
    }

    if (arg.IsSwitchOn("-O"))
    {
        ConstantPropagator cp;
        int counter = 0;
        do
        {
            counter++;
            if (counter >= 10)
                break;

            cp.didTreeChange = false;

            for (auto &stmt : parsed)
                stmt->Evaluate();

            for (auto &stmt : parsed)
                stmt->Propagate(cp);

            std::cout << "didTreeChange = " << cp.didTreeChange << std::endl;
        } while (cp.didTreeChange);
        std::cout << "count = " << counter << std::endl;
    }

    Compiler c;
    c.Compile(parsed);

    if (arg.IsSwitchOn("-c"))
    {
        std::cout << "\n\nCOMPILED" << std::endl;
        c.Disassemble();
    }

    Verifier v;
    v.SetFunction(c.Functions[1]);
    v.GenerateStrongestPost(dynamic_cast<FuncDecl *>(parsed[0].get())->preConds);

    // langle 〈
    ASTPrinter ast(false);
    ast << "[";
    for (size_t i = 0; i < v.post.size(); i++)
    {
        std::vector<std::shared_ptr<Expr>> conj = v.post[i];

        ast << "\u3008";
        for (size_t j = 0; j < conj.size(); j++)
        {
            conj[j]->Print(ast);

            if (j != conj.size() - 1)
                ast << " && ";
        }
        ast << "\u3009";

        if (i != v.post.size() - 1)
            ast << " || ";
    }
    // rangle 〉
    ast << "]"
        << "\n";
    ast.Flush();
    std::string ofPath = arg.GetArgVal("-o");

    if (arg.IsSwitchOn("--rm-bin"))
    {
        std::string rm = "rm -f " + ofPath;
        int sysCode = system(rm.c_str());
        if (sysCode == -1)
            std::cerr << "Command to remove serialisation of program failed" << std::endl;
    }
    Compiler::SerialiseProgram(c, ofPath);
    return 0;
}