#pragma once
#include "postcondition.h"
#include "staticanalyser.h"

class Verifier
{

public:
    Verifier() = default;

    std::vector<std::vector<SP<Expr>>> GeneratePost(SP<FuncDecl> fd, StaticAnalyser &sa);
    
};