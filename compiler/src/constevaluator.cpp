#include "constevaluator.h"

#define IS(x)                     \
    x(LITERAL, Expr);             \
    x(UNARY, Expr);               \
    x(BINARY, Expr);              \
    x(VAR_REFERENCE, Expr);       \
    x(ASSIGN, Expr);              \
    x(FUNCTION_CALL, Expr);       \
    x(ARRAY_INDEX, Expr);         \
    x(BRACED_INITIALISER, Expr);  \
    x(DYNAMIC_ALLOC_ARRAY, Expr); \
    x(FIELD_ACCESS, Expr);        \
    x(TYPE_CAST, Expr);           \
    x(SEQUENCE, Expr);            \
    x(EXPR_STMT, Stmt);           \
    x(DECLARED_VAR, Stmt);        \
    x(BLOCK, Stmt);               \
    x(IF_STMT, Stmt);             \
    x(WHILE_STMT, Stmt);          \
    x(FUNC_DECL, Stmt);           \
    x(RETURN, Stmt);              \
    x(STRUCT_DECL, Stmt);         \
    x(IMPORT_STMT, Stmt);         \
    x(BREAK, Stmt);               \
    x(THROW, Stmt);               \
    x(TRY_CATCH, Stmt);

#define x(nodekind, node)                           \
    inline bool IS_##nodekind(const SP<node> &expr) \
    {                                               \
        return expr->kind == node##Kind::nodekind;  \
    }

IS(x);
#undef x
#undef Kind

inline SP<Expr> OPERATE(const SP<Expr> &left, const TokenID op, const SP<Expr> &right)
{
    Token loc = left->Loc();
    loc.type = op;
    return std::make_shared<Binary>(left, loc, right);
}

SP<Expr> ConstantEvaluator::SimplifyExpression(const SP<Expr> &expr)
{
    switch (expr->kind)
    {
    case ExprKind::LITERAL:
    {
        return expr;
    }
    case ExprKind::UNARY:
    {
        SP<Unary> u = std::dynamic_pointer_cast<Unary>(expr);
        u->right = SimplifyExpression(u->right);

        if (!IS_LITERAL(u->right))
            return expr;

        if (u->op.type == TokenID::MINUS)
            return UNARY_MINUS(u);
        else if (u->op.type == TokenID::BANG)
            return UNARY_BANG(u);

        return u;
    }
    case ExprKind::BINARY:
    {
        SP<Binary> b = std::dynamic_pointer_cast<Binary>(expr);
        b->left = SimplifyExpression(b->left);
        b->right = SimplifyExpression(b->right);

        if (!IS_LITERAL(b->left) || !IS_LITERAL(b->right))
        {
            if (IS_SEQUENCE(b->left) &&
                std::dynamic_pointer_cast<Sequence>(b->left)->op == b->op.type)
                return BINARY_SEQUENCE(b, true);
            else if (IS_SEQUENCE(b->right) &&
                     std::dynamic_pointer_cast<Sequence>(b->right)->op == b->op.type)
                return BINARY_SEQUENCE(b, false);
            return expr;
        }

        if (b->op.type == TokenID::PLUS)
            return BINARY_PLUS(b);
        else if (b->op.type == TokenID::MINUS)
            return BINARY_MINUS(b);
        else if (b->op.type == TokenID::STAR)
            return BINARY_STAR(b);
        else if (b->op.type == TokenID::SLASH)
            return BINARY_SLASH(b);
        else if (b->op.type == TokenID::GT)
            return BINARY_GT(b);
        else if (b->op.type == TokenID::LT)
            return BINARY_LT(b);
        else if (b->op.type == TokenID::GEQ)
            return BINARY_GEQ(b);
        else if (b->op.type == TokenID::LEQ)
            return BINARY_LEQ(b);
        else if (b->op.type == TokenID::EQ_EQ)
            return BINARY_EQ_EQ(b);
        else if (b->op.type == TokenID::BANG_EQ)
            return BINARY_BANG_EQ(b);
        else if (b->op.type == TokenID::AND_AND)
            return BINARY_AND_AND(b);
        else if (b->op.type == TokenID::OR_OR)
            return BINARY_OR_OR(b);

        return b;
    }
    case ExprKind::VAR_REFERENCE:
    {
        return expr;
    }
    case ExprKind::ASSIGN:
    {
        SP<Assign> a = std::dynamic_pointer_cast<Assign>(expr);
        a->val = SimplifyExpression(a->val);
        return a;
    }
    case ExprKind::FUNCTION_CALL:
    {
        SP<FunctionCall> fc = std::dynamic_pointer_cast<FunctionCall>(expr);
        for (auto &arg : fc->args)
            arg = SimplifyExpression(arg);
        return fc;
    }
    case ExprKind::ARRAY_INDEX:
    {
        SP<ArrayIndex> ai = std::dynamic_pointer_cast<ArrayIndex>(expr);
        ai->index = SimplifyExpression(ai->index);
        return ai;
    }
    case ExprKind::BRACED_INITIALISER:
    {
        SP<BracedInitialiser> bi = std::dynamic_pointer_cast<BracedInitialiser>(expr);
        for (auto &init : bi->init)
            init = SimplifyExpression(init);
        return bi;
    }
    case ExprKind::DYNAMIC_ALLOC_ARRAY:
    {
        SP<DynamicAllocArray> da = std::dynamic_pointer_cast<DynamicAllocArray>(expr);
        da->size = SimplifyExpression(da->size);
        return da;
    }
    case ExprKind::FIELD_ACCESS:
    {
        return expr;
    }
    case ExprKind::TYPE_CAST:
    {
        SP<TypeCast> tc = std::dynamic_pointer_cast<TypeCast>(expr);
        tc->arg = SimplifyExpression(tc->arg);
        return tc;
    }
    case ExprKind::SEQUENCE:
    {
        SP<Sequence> s = std::dynamic_pointer_cast<Sequence>(expr);
        s->start = SimplifyExpression(s->start);
        s->step = SimplifyExpression(s->step);
        s->end = SimplifyExpression(s->end);
        s->term = SimplifyExpression(s->term);
        return s;
    }
    }
    return nullptr;
}

void ConstantEvaluator::SimplifyStatement(const SP<Stmt> &stmt)
{
    switch (stmt->kind)
    {
    case StmtKind::EXPR_STMT:
    {
        SP<ExprStmt> es = std::dynamic_pointer_cast<ExprStmt>(stmt);
        es->exp = SimplifyExpression(es->exp);
        break;
    }
    case StmtKind::DECLARED_VAR:
    {
        SP<DeclaredVar> dv = std::dynamic_pointer_cast<DeclaredVar>(stmt);
        dv->value = SimplifyExpression(dv->value);
        break;
    }
    case StmtKind::BLOCK:
    {
        SP<Block> b = std::dynamic_pointer_cast<Block>(stmt);
        for (auto &stmt : b->stmts)
            SimplifyStatement(stmt);
        break;
    }
    case StmtKind::IF_STMT:
    {
        SP<IfStmt> i = std::dynamic_pointer_cast<IfStmt>(stmt);
        i->cond = SimplifyExpression(i->cond);
        SimplifyStatement(i->then_branch);
        SimplifyStatement(i->else_branch);
        break;
    }
    case StmtKind::WHILE_STMT:
    {
        SP<WhileStmt> ws = std::dynamic_pointer_cast<WhileStmt>(stmt);
        ws->cond = SimplifyExpression(ws->cond);
        SimplifyStatement(ws->body);
        break;
    }
    case StmtKind::FUNC_DECL:
    {
        SP<FuncDecl> fd = std::dynamic_pointer_cast<FuncDecl>(stmt);
        for (auto &stmt : fd->body)
            SimplifyStatement(stmt);
        break;
    }
    case StmtKind::RETURN:
    {
        SP<Return> r = std::dynamic_pointer_cast<Return>(stmt);
        r->ret_val = SimplifyExpression(r->ret_val);
        break;
    }
    case StmtKind::THROW:
    {
        SP<Throw> t = std::dynamic_pointer_cast<Throw>(stmt);
        t->exp = SimplifyExpression(t->exp);
        break;
    }
    case StmtKind::TRY_CATCH:
    {
        SP<TryCatch> tc = std::dynamic_pointer_cast<TryCatch>(stmt);
        SimplifyStatement(tc->try_clause);
        SimplifyStatement(tc->catch_clause);
        break;
    }
    default:
    {
        break;
    }
    }
}

inline std::string GET_STRING_LITERAL(const SP<Literal> &l)
{
    return l->Loc().literal;
}

inline bool STRING_TO_BOOL(const std::string &s)
{
    return s == "true";
}

inline char STRING_TO_CHAR(const std::string &s)
{
    return s[0];
}

#define GETTER(x)                    \
    x(int, INT, std::stoi);          \
    x(double, DOUBLE, std::stod);    \
    x(bool, BOOL, STRING_TO_BOOL);   \
    x(std::string, STRING, NOTHING); \
    x(char, CHAR, STRING_TO_CHAR);
#define x(ret, type, converter)                  \
    inline ret GET_##type(const SP<Literal> &l)  \
    {                                            \
        return converter(GET_STRING_LITERAL(l)); \
    }
#define NOTHING
GETTER(x)
#undef x
#undef NOTHING

SP<Expr> ConstantEvaluator::UNARY_MINUS(const SP<Unary> &u)
{
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(u->right);
    if (right->t != INT_TYPE && right->t != DOUBLE_TYPE)
        return u;

    if (right->t == INT_TYPE)
        return std::make_shared<Literal>(right->Loc(), -GET_INT(right));
    else
        return std::make_shared<Literal>(right->Loc(), -GET_DOUBLE(right));
}

SP<Expr> ConstantEvaluator::UNARY_BANG(const SP<Unary> &u)
{
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(u->right);
    if (right->t != BOOL_TYPE)
        return u;

    return std::make_shared<Literal>(right->Loc(), !GET_BOOL(right));
}

SP<Expr> ConstantEvaluator::BINARY_SEQUENCE(const SP<Binary> &b, bool left_seq)
{
    SP<Sequence> seq;
    SP<Expr> val;
    if (left_seq)
    {
        seq = std::dynamic_pointer_cast<Sequence>(b->left);
        val = b->right;
    }
    else
    {
        seq = std::dynamic_pointer_cast<Sequence>(b->right);
        val = b->left;
    }

    SP<Expr> low = OPERATE(seq->start, TokenID::MINUS, seq->step);
    SP<Expr> high = OPERATE(seq->end, TokenID::PLUS, seq->step);

    low = SimplifyExpression(low);
    high = SimplifyExpression(high);

    SP<Expr> index = seq->var;
    SP<Expr> low_term = NodeSubstituter::Substitute(seq->term, index, low);
    SP<Expr> high_term = NodeSubstituter::Substitute(seq->term, index, high);

    low_term = SimplifyExpression(low_term);
    high_term = SimplifyExpression(high_term);

    bool did_simp_seq = false;

    if (NodeEqual::Equal(high_term, val))
    {
        seq->end = high;
        did_simp_seq = true;
    }
    else if (NodeEqual::Equal(low_term, val))
    {
        seq->start = low;
        did_simp_seq = true;
    }

    if (did_simp_seq)
        return seq;
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_PLUS(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    if (left->t == INT_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::INT_L;
        return std::make_shared<Literal>(loc, GET_INT(left) + GET_INT(right));
    }
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_INT(left) + GET_DOUBLE(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) + GET_INT(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) + GET_DOUBLE(right));
    }
    else if (left->t == STRING_TYPE && right->t == STRING_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_STRING_LITERAL(left) + GET_STRING_LITERAL(right));
    }
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_MINUS(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    if (left->t == INT_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::INT_L;
        return std::make_shared<Literal>(loc, GET_INT(left) - GET_INT(right));
    }
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_INT(left) - GET_DOUBLE(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) - GET_INT(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) - GET_DOUBLE(right));
    }
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_STAR(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    if (left->t == INT_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::INT_L;
        return std::make_shared<Literal>(loc, GET_INT(left) * GET_INT(right));
    }
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_INT(left) * GET_DOUBLE(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) * GET_INT(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) * GET_DOUBLE(right));
    }
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_SLASH(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    if (left->t == INT_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::INT_L;
        return std::make_shared<Literal>(loc, GET_INT(left) / GET_INT(right));
    }
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_INT(left) / GET_DOUBLE(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) / GET_INT(right));
    }
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
    {
        loc.type = TokenID::DOUBLE_L;
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) / GET_DOUBLE(right));
    }
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_GT(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == INT_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) > GET_INT(right));
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) > GET_DOUBLE(right));
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) > GET_INT(right));
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) > GET_DOUBLE(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_LT(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == INT_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) < GET_INT(right));
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) < GET_DOUBLE(right));
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) < GET_INT(right));
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) < GET_DOUBLE(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_GEQ(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == INT_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) >= GET_INT(right));
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) >= GET_DOUBLE(right));
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) >= GET_INT(right));
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) >= GET_DOUBLE(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_LEQ(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == INT_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) <= GET_INT(right));
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) <= GET_DOUBLE(right));
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) <= GET_INT(right));
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) <= GET_DOUBLE(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_EQ_EQ(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == INT_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) == GET_INT(right));
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) == GET_DOUBLE(right));
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) == GET_INT(right));
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) == GET_DOUBLE(right));
    else if (left->t == BOOL_TYPE && right->t == BOOL_TYPE)
        return std::make_shared<Literal>(loc, GET_BOOL(left) == GET_BOOL(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_BANG_EQ(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == INT_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) != GET_INT(right));
    else if (left->t == INT_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_INT(left) != GET_DOUBLE(right));
    else if (left->t == DOUBLE_TYPE && right->t == INT_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) != GET_INT(right));
    else if (left->t == DOUBLE_TYPE && right->t == DOUBLE_TYPE)
        return std::make_shared<Literal>(loc, GET_DOUBLE(left) != GET_DOUBLE(right));
    else if (left->t == BOOL_TYPE && right->t == BOOL_TYPE)
        return std::make_shared<Literal>(loc, GET_BOOL(left) != GET_BOOL(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_AND_AND(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == BOOL_TYPE && right->t == BOOL_TYPE)
        return std::make_shared<Literal>(loc, GET_BOOL(left) && GET_BOOL(right));
    else
        return b;
}

SP<Expr> ConstantEvaluator::BINARY_OR_OR(const SP<Binary> &b)
{
    SP<Literal> left = std::dynamic_pointer_cast<Literal>(b->left);
    SP<Literal> right = std::dynamic_pointer_cast<Literal>(b->right);

    Token loc = left->Loc();
    loc.type = TokenID::BOOL_L;
    if (left->t == BOOL_TYPE && right->t == BOOL_TYPE)
        return std::make_shared<Literal>(loc, GET_BOOL(left) || GET_BOOL(right));
    else
        return b;
}