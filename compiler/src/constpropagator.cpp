#include "constpropagator.h"

bool operator==(const VarRef &lhs, const VarRef &rhs)
{
    return (lhs.type == rhs.type) && (lhs.name == rhs.name);
}

void ConstantPropagator::Propagate(std::vector<SP<Stmt>> &program)
{
    for (auto &stmt : program)
        PropagateStatement(stmt);
}

SP<Expr> ConstantPropagator::PropagateExpression(SP<Expr> &expr)
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
        u->right = PropagateExpression(u->right);
        return u;
    }
    case ExprKind::BINARY:
    {
        SP<Binary> b = std::dynamic_pointer_cast<Binary>(expr);
        b->left = PropagateExpression(b->left);
        b->right = PropagateExpression(b->right);
        return b;
    }
    case ExprKind::VAR_REFERENCE:
    {
        SP<VarReference> vr = std::dynamic_pointer_cast<VarReference>(expr);
        std::optional<VarID> vid = symbols.GetVar(vr->name);

        SP<Expr> val = constants.GetVarVal(vid->type, vid->name);

        if (val != nullptr)
        {
            didPropagate = true;
            return val;
        }
        return expr;
    }
    case ExprKind::ASSIGN:
    {
        SP<Assign> a = std::dynamic_pointer_cast<Assign>(expr);
        a->target = PropagateExpression(a->target);

        // TODO - Update this to be more correct and competent
        VarReference *targetAsVR = dynamic_cast<VarReference *>(a->target.get());
        if (targetAsVR != nullptr)
        {
            std::optional<VarID> vid = symbols.GetVar(targetAsVR->name);
            constants.RemoveVarVal(vid->type, vid->name);
        }
        return a;
    }
    case ExprKind::FUNCTION_CALL:
    {
        SP<FunctionCall> fc = std::dynamic_pointer_cast<FunctionCall>(expr);
        for (auto &arg : fc->args)
            arg = PropagateExpression(arg);
        return fc;
    }
    case ExprKind::ARRAY_INDEX:
    {
        SP<ArrayIndex> ai = std::dynamic_pointer_cast<ArrayIndex>(expr);
        ai->index = PropagateExpression(ai->index);
        return ai;
    }
    case ExprKind::BRACED_INITIALISER:
    {
        SP<BracedInitialiser> bi = std::dynamic_pointer_cast<BracedInitialiser>(expr);
        for (auto init : bi->init)
            init = PropagateExpression(init);
        return bi;
    }
    case ExprKind::DYNAMIC_ALLOC_ARRAY:
    {
        SP<DynamicAllocArray> da = std::dynamic_pointer_cast<DynamicAllocArray>(expr);
        da->size = PropagateExpression(da->size);
        return da;
    }
    case ExprKind::FIELD_ACCESS:
    {
        return expr;
    }
    case ExprKind::TYPE_CAST:
    {
        SP<TypeCast> tc = std::dynamic_pointer_cast<TypeCast>(expr);
        tc->arg = PropagateExpression(tc->arg);
        return tc;
    }
    case ExprKind::SEQUENCE:
    {
        SP<Sequence> s = std::dynamic_pointer_cast<Sequence>(expr);
        s->start = PropagateExpression(s->start);
        s->step = PropagateExpression(s->step);
        s->end = PropagateExpression(s->end);
        s->term = PropagateExpression(s->term);
        return s;
    }
    }
    // dummy return
    return nullptr;
}

void ConstantPropagator::PropagateStatement(SP<Stmt> &stmt)
{
    switch (stmt->kind)
    {
    case StmtKind::EXPR_STMT:
    {
        ExprStmt *es = dynamic_cast<ExprStmt *>(stmt.get());
        es->exp = PropagateExpression(es->exp);
        break;
    }
    case StmtKind::DECLARED_VAR:
    {
        DeclaredVar *dv = dynamic_cast<DeclaredVar *>(stmt.get());
        symbols.AddVar(dv->t, dv->name, 0);

        if (dv->value != nullptr)
        {
            dv->value = PropagateExpression(dv->value);
            if (dynamic_cast<Literal *>(dv->value.get()) != nullptr)
                constants.AddVar(dv->t, dv->name, dv->value);
        }
        break;
    }
    case StmtKind::BLOCK:
    {
        Block *b = dynamic_cast<Block *>(stmt.get());

        symbols.depth++;
        constants.depth++;

        for (auto &stmt : b->stmts)
            PropagateStatement(stmt);

        constants.ClearCurrentDepth();
        symbols.CleanUpCurDepth();

        symbols.depth--;
        constants.depth--;
        break;
    }
    case StmtKind::IF_STMT:
    {
        IfStmt *i = dynamic_cast<IfStmt *>(stmt.get());

        symbols.depth++;
        constants.depth++;
        i->cond = PropagateExpression(i->cond);

        constants.ClearCurrentDepth();
        symbols.CleanUpCurDepth();

        symbols.depth--;
        constants.depth--;

        PropagateStatement(i->then_branch);
        if (i->else_branch != nullptr)
        {
            symbols.depth++;
            constants.depth++;

            PropagateStatement(i->else_branch);

            constants.ClearCurrentDepth();
            symbols.CleanUpCurDepth();

            symbols.depth--;
            constants.depth--;
        }
        break;
    }
    case StmtKind::WHILE_STMT:
    {
        WhileStmt *ws = dynamic_cast<WhileStmt *>(stmt.get());

        symbols.depth++;
        constants.depth++;

        PropagateStatement(ws->body);

        constants.ClearCurrentDepth();
        symbols.CleanUpCurDepth();

        symbols.depth--;
        constants.depth--;

        break;
    }
    case StmtKind::FUNC_DECL:
    {
        FuncDecl *fd = dynamic_cast<FuncDecl *>(stmt.get());
        symbols.depth++;
        constants.depth++;
        for (auto &stmt : fd->body)
            PropagateStatement(stmt);

        constants.ClearCurrentDepth();
        symbols.CleanUpCurDepth();

        symbols.depth--;
        constants.depth--;
        break;
    }
    case StmtKind::RETURN:
    {
        Return *r = dynamic_cast<Return *>(stmt.get());
        if (r->ret_val != nullptr)
            r->ret_val = PropagateExpression(r->ret_val);
        break;
    }
    case StmtKind::THROW:
    {
        Throw *t = dynamic_cast<Throw *>(stmt.get());
        t->exp = PropagateExpression(t->exp);
        break;
    }
    case StmtKind::TRY_CATCH:
    {
        TryCatch *tc = dynamic_cast<TryCatch *>(stmt.get());

        symbols.depth++;
        constants.depth++;
        PropagateStatement(tc->try_clause);

        constants.ClearCurrentDepth();
        symbols.CleanUpCurDepth();

        PropagateStatement(tc->catch_clause);
        symbols.depth--;
        constants.depth--;
        break;
    }
    default:
    {
        break;
    }
    }
}