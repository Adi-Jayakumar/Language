#include "nodeequal.h"
#include "stmtnode.h"

namespace ConstantEvaluator
{
    SP<Expr> SimplifyExpression(SP<Expr> &expr);
    void SimplifyStatement(SP<Stmt> &stmt);
    void Simplify(std::vector<SP<Stmt>> &program);

    SP<Expr> UNARY_MINUS(SP<Unary> &u);
    SP<Expr> UNARY_BANG(SP<Unary> &u);

    SP<Expr> BINARY_SEQUENCE(SP<Binary> &b, bool leftSeq);
    SP<Expr> BINARY_PLUS(SP<Binary> &b);
    SP<Expr> BINARY_MINUS(SP<Binary> &b);
    SP<Expr> BINARY_STAR(SP<Binary> &b);
    SP<Expr> BINARY_SLASH(SP<Binary> &b);
    SP<Expr> BINARY_GT(SP<Binary> &b);
    SP<Expr> BINARY_LT(SP<Binary> &b);
    SP<Expr> BINARY_GEQ(SP<Binary> &b);
    SP<Expr> BINARY_LEQ(SP<Binary> &b);
    SP<Expr> BINARY_EQ_EQ(SP<Binary> &b);
    SP<Expr> BINARY_BANG_EQ(SP<Binary> &b);
    SP<Expr> BINARY_AND_AND(SP<Binary> &b);
    SP<Expr> BINARY_OR_OR(SP<Binary> &b);
}