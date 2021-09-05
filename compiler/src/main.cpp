#include "ASTPrinter.h"
#include "argparser.h"
#include "compiler.h"
#include "constevaluator.h"
#include "nodesubstitution.h"
#include "parser.h"
#include "postcondition.h"
#include "serialise.h"
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

void PrintPost(std::vector<std::vector<SP<Expr>>> &post)
{
    ASTPrinter ast(false);
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
    arg.AddSwitch("-c");
    arg.AddSwitch("--rm-bin");

    arg.AddArg({"-f", "-o"});

    arg.ParseArgs(argc - 1, argv + 1);

    std::string ifPath = arg.GetArgVal("-f");

    Parser p(ifPath);
    std::vector<SP<Stmt>> parsed = p.Parse();

    if (p.hadError)
        exit(2);

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

    StaticAnalyser sa;
    sa.Analyse(parsed);

    Verifier v;
    std::vector<std::vector<SP<Expr>>> post = v.GeneratePost(std::dynamic_pointer_cast<FuncDecl>(parsed[0]), sa);
    PrintPost(post);

    // SP<Expr> ten = std::make_shared<Literal>(10);
    // SP<Expr> hund = std::make_shared<Literal>(100);
    // Token loc = ten->Loc();
    // loc.type = TokenID::STAR;
    // SP<Expr> mult = std::make_shared<Binary>(ten, loc, ten);

    // ConstantPropagator cp;

    // for (auto &stmt : parsed)
    // {
    //     cp.PropagateStatement(stmt);
    //     // ConstantEvaluator::SimplifyStatement(stmt);
    //     // replaced.push_back(NodeSubstituter::Substitute(stmt, mult, hund));
    // }

    // if (arg.IsSwitchOn("-p"))
    // {
    //     std::cout << "PARSED" << std::endl;
    //     ASTPrinter ast(false);

    //     for (auto &stmt : parsed)
    //         stmt->Print(ast);
    //     ast.Flush();
    // }

    Compiler c;
    c.Compile(parsed);

    if (c.hadError)
        exit(3);

    if (arg.IsSwitchOn("-c"))
    {
        std::cout << "\n\nCOMPILED" << std::endl;
        c.Disassemble();
    }

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