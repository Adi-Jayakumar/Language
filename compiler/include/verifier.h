#pragma once
#include "exprnode.h"
#include "function.h"
#include "perror.h"

enum class ConditionOperation : uint8_t
{
    POP,
    NEGATE_TOP
};

std::string ToString(ConditionOperation c);
std::ostream &operator<<(std::ostream &out, ConditionOperation c);

struct ConditionStackOp
{
    ConditionOperation op;
    size_t idx;
    ConditionStackOp(ConditionOperation _op, size_t _idx) : op(_op), idx(_idx){};
};

// TODO - In static analysis ensure that top level
// verification expressions are only of types
// 'VarReference' or 'Assign' and that the number of
// pre conditions is equal to the number of arguments
class Verifier
{
    std::vector<std::shared_ptr<Expr>> stack;
    std::vector<std::shared_ptr<Expr>> conditions;
    std::vector<ConditionStackOp> conditionPop;
    Function f;

public:
    std::vector<std::vector<std::shared_ptr<Expr>>> post;
    Verifier() = default;
    Verifier(Function &_f) : f(_f){};

    void VerificationError(std::string msg);
    void SetFunction(const Function &_f)
    {
        f = _f;
    }

    std::vector<std::shared_ptr<Expr>> GetAssumptions();

    // populates 'post' with the strongest post condition for the
    // supplied pre condition
    void GenerateStrongestPost(std::vector<std::shared_ptr<Expr>> &pre);
};