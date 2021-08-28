#include "ASTPrinter.h"
#include "argparser.h"
#include "compiler.h"
#include "nodesubstitution.h"
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

    ArgParser arg;

    arg.AddSwitch("-l");
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

    if (arg.IsSwitchOn("-l"))
    {
        std::cout << "Dumping tokens" << std::endl;
        DumpTokens(ifPath);
    }

    if (arg.IsSwitchOn("-p"))
    {
        std::cout << "PARSED" << std::endl;
        ASTPrinter ast(false);

        for (auto &stmt : parsed)
            stmt->Print(ast);
        ast.Flush();
    }

    // StaticAnalyser sa;
    // sa.Analyse(parsed);

    // Compiler c;
    // c.Compile(parsed);

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