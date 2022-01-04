#include "ASTPrinter.h"
#include "argparser.h"
#include "compiler.h"
#include "constevaluator.h"
#include "nodesubstitution.h"
#include "parser.h"
#include "serialise.h"
#include "staticanalyser.h"
#include "verifier.h"

void DumpTokens(std::string fPath, SymbolTable &symbols)
{
    // Lexer l = Lexer(fPath);
    Parser p(fPath, symbols);

    while (p.cur.type != TokenID::END)
    {
        std::cout << p.cur << std::endl;
        p.Advance();
    }
    std::cout << p.cur << std::endl;
}

void PrintPost(std::vector<std::vector<SP<Expr>>> &post, SymbolTable &symbols)
{
    ASTPrinter ast(false, symbols);
    ast << "[";
    for (size_t i = 0; i < post.size(); i++)
    {
        std::vector<std::shared_ptr<Expr>> conj = post[i];

        // langle 〈
        ast << "\u3008";
        for (size_t j = 0; j < conj.size(); j++)
        {
            conj[j]->Print(ast);

            if (j != conj.size() - 1)
                ast << " && ";
        }
        ast << "\u3009";
        // rangle 〉

        if (i != post.size() - 1)
            ast << " || ";
    }
    ast << "]"
        << "\n";
    ast.Flush();
}

#ifdef COMPILE_FOR_TEST
#define main not_main
#endif

int main(int argc, char **argv)
{

    ArgParser arg;

    arg.AddSwitch("-l");
    arg.AddSwitch("-p");
    arg.AddSwitch("-t");
    arg.AddSwitch("-O");
    arg.AddSwitch("-exp");
    arg.AddSwitch("-c");
    arg.AddSwitch("--rm-bin");

    arg.AddArg({"-f", "-o"});
    arg.ParseArgs(argc - 1, argv + 1);

    std::string ifPath = arg.GetArgVal("-f");

    SymbolTable symbols;

    if (arg.IsSwitchOn("-l"))
    {
        std::cout << "Dumping tokens" << std::endl;
        DumpTokens(ifPath, symbols);
    }

    Parser p(ifPath, symbols);
    std::vector<SP<Stmt>> parsed = p.Parse();

    if (p.had_error)
        exit(2);

    if (arg.IsSwitchOn("-p"))
    {
        std::cout << "PARSED\n\n"
                  << std::endl;
        ASTPrinter ast(false, symbols);

        for (auto &stmt : parsed)
            stmt->Print(ast);
        ast.Flush();
    }

    // StaticAnalyser sa(symbols);
    // sa.Analyse(parsed);

    // if (arg.IsSwitchOn("-t"))
    // {
    //     std::cout << "ANALYSED" << std::endl;
    //     ASTPrinter ast(true, symbols);

    //     for (auto &stmt : parsed)
    //         stmt->Print(ast);
    //     ast.Flush();
    // }

    // if (arg.IsSwitchOn("-exp"))
    // {
    //     Verifier v(symbols);
    //     v.Generate(dynamic_cast<FuncDecl *>(parsed[0].get()));
    // }

    // Compiler c;
    // c.Compile(parsed);

    // if (c.had_error)
    //     exit(3);

    // if (arg.IsSwitchOn("-c"))
    // {
    //     std::cout << "\n\nCOMPILED" << std::endl;
    //     c.Disassemble();
    // }

    // std::string ofPath = arg.GetArgVal("-o");

    // if (arg.IsSwitchOn("--rm-bin"))
    // {
    //     std::string rm = "rm -f " + ofPath;
    //     int sysCode = system(rm.c_str());
    //     if (sysCode == -1)
    //         std::cerr << "Command to remove serialisation of program failed" << std::endl;
    // }
    // Compiler::SerialiseProgram(c, ofPath);
    return 0;
}