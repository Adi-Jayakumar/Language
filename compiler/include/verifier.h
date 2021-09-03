#pragma once
#include "postcondition.h"
#include "stmtnode.h"

class Verifier
{
    //...
public:
    Verifier() = default;
    void Verify(std::vector<SP<Stmt>> &_program);
};