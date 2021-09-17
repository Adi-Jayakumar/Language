#include "verifier.h"

std::vector<std::vector<SP<Expr>>> Verifier::GeneratePost(SP<FuncDecl> fd, StaticAnalyser &sa)
{
    PostConditionGenerator pc(sa.GetSymbolTable());
    std::vector<std::vector<SP<Expr>>> post = pc.Generate(fd, program);
    return post;
}

