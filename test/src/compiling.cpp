#include "compiler.h"
#include "parser.h"
#include "staticanalyser.h"

inline bool FileExists(std::string file)
{
    return std::ifstream(file).good();
}

void CompileFile(std::string file, std::string out)
{
    Parser p("scripts/" + file + ".txt");
    std::vector<std::shared_ptr<Stmt>> program = p.Parse();
    StaticAnalyser s;

    for (auto &stmt : program)
        stmt->Type(s);

    Compiler c;
    c.Compile(program);
    
    if (!FileExists(out))
        Compiler::SerialiseProgram(c, out);
}