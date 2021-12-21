#include "nodeequal.h"
#include "nodesubstitution.h"
#include "stmtnode.h"

namespace ConstantEvaluator
{
    SP<Expr> SimplifyExpression(const SP<Expr> &expr);
    void SimplifyStatement(const SP<Stmt> &stmt);
    void Simplify(std::vector<SP<Stmt>> &program);

    SP<Expr> UNARY_MINUS(const SP<Unary> &u);
    SP<Expr> UNARY_BANG(const SP<Unary> &u);

    SP<Expr> BINARY_SEQUENCE(const SP<Binary> &b, bool leftSeq);
    SP<Expr> BINARY_PLUS(const SP<Binary> &b);
    SP<Expr> BINARY_MINUS(const SP<Binary> &b);
    SP<Expr> BINARY_STAR(const SP<Binary> &b);
    SP<Expr> BINARY_SLASH(const SP<Binary> &b);
    SP<Expr> BINARY_GT(const SP<Binary> &b);
    SP<Expr> BINARY_LT(const SP<Binary> &b);
    SP<Expr> BINARY_GEQ(const SP<Binary> &b);
    SP<Expr> BINARY_LEQ(const SP<Binary> &b);
    SP<Expr> BINARY_EQ_EQ(const SP<Binary> &b);
    SP<Expr> BINARY_BANG_EQ(const SP<Binary> &b);
    SP<Expr> BINARY_AND_AND(const SP<Binary> &b);
    SP<Expr> BINARY_OR_OR(const SP<Binary> &b);
}