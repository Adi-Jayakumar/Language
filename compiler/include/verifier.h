#pragma once
#include "postcondition.h"
#include "staticanalyser.h"

class Verifier
{

    std::vector<SP<Stmt>> program;

public:
    Verifier(const std::vector<SP<Stmt>> &_program) : program(_program){};

    std::vector<std::vector<SP<Expr>>> GeneratePost(SP<FuncDecl> fd, StaticAnalyser &sa);
};