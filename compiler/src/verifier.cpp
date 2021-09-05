#include "verifier.h"

std::vector<std::vector<SP<Expr>>> Verifier::GeneratePost(SP<FuncDecl> fd, StaticAnalyser &sa)
{
    PostConditionGenerator pc;
    std::vector<std::vector<SP<Expr>>> post = pc.Generate(fd);
    sa.AnalysePost(fd.get(), post);

    return post;
}

