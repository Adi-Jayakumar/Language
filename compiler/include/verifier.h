#pragma once
#include "exprnode.h"
#include "function.h"
#include "perror.h"
#include <stack>

// TODO - In static analysis ensure that top level
// verification expressions are only of types
// 'VarReference' or 'Assign' and that the number of
// pre conditions is equal to the number of arguments
class Verifier
{
    std::vector<std::shared_ptr<Expr>> stack;
    Function f;

    size_t ip;

public:
    std::vector<std::shared_ptr<Expr>> post;
    Verifier() = default;
    Verifier(Function &_f) : f(_f){};

    void VerificationError(std::string msg);

    void SetFunction(const Function &_f)
    {
        f = _f;
    }

    // populates 'post' with the strongest post condition for the
    // supplied pre condition
    void GenerateStrongestPost(std::vector<std::shared_ptr<Expr>> &pre);
};