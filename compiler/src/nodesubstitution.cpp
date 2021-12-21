#include "nodesubstitution.h"

#define COPY(type, node) \
    std::make_shared<type>(*std::dynamic_pointer_cast<type>(node))

SP<Expr> NodeSubstituter::Substitute(SP<Expr> &tree, SP<Expr> &node, SP<Expr> &val)
{
    if (NodeEqual::Equal(node, val))
        return tree;

    if (NodeEqual::Equal(tree, node))
        return val;

    switch (tree->kind)
    {
    case ExprKind::LITERAL:
    {
        return tree;
    }
    case ExprKind::UNARY:
    {
        SP<Unary> u = COPY(Unary, tree);
        u->right = Substitute(u->right, node, val);
        return u;
    }
    case ExprKind::BINARY:
    {
        SP<Binary> b = COPY(Binary, tree);
        b->left = Substitute(b->left, node, val);
        b->right = Substitute(b->right, node, val);
        return b;
    }
    case ExprKind::VAR_REFERENCE:
    {
        return tree;
    }
    case ExprKind::ASSIGN:
    {
        SP<Assign> a = COPY(Assign, tree);
        a->target = Substitute(a->target, node, val);
        a->val = Substitute(a->val, node, val);
        return a;
    }
    case ExprKind::FUNCTION_CALL:
    {
        SP<FunctionCall> fc = COPY(FunctionCall, tree);
        for (auto &arg : fc->args)
            arg = Substitute(arg, node, val);
        return fc;
    }
    case ExprKind::ARRAY_INDEX:
    {
        SP<ArrayIndex> ai = COPY(ArrayIndex, tree);
        ai->name = Substitute(ai->name, node, val);
        ai->index = Substitute(ai->index, node, val);
        return ai;
    }
    case ExprKind::BRACED_INITIALISER:
    {
        SP<BracedInitialiser> bi = COPY(BracedInitialiser, tree);
        for (auto &init : bi->init)
            init = Substitute(init, node, val);
        return bi;
    }
    case ExprKind::DYNAMIC_ALLOC_ARRAY:
    {
        SP<DynamicAllocArray> da = COPY(DynamicAllocArray, tree);
        da->size = Substitute(da->size, node, val);
        return da;
    }
    case ExprKind::FIELD_ACCESS:
    {
        SP<FieldAccess> fa = COPY(FieldAccess, tree);
        fa->accessee = Substitute(fa->accessee, node, val);
        fa->accessor = Substitute(fa->accessor, node, val);
        return fa;
    }
    case ExprKind::TYPE_CAST:
    {
        SP<TypeCast> tc = COPY(TypeCast, tree);
        tc->arg = Substitute(tc->arg, node, val);
        return tc;
    }
    case ExprKind::SEQUENCE:
    {
        SP<Sequence> s = COPY(Sequence, tree);
        s->start = Substitute(s->start, node, val);
        s->step = Substitute(s->step, node, val);
        s->end = Substitute(s->end, node, val);
        s->term = Substitute(s->term, node, val);
        return s;
    }
    }
    return tree;
}

SP<Stmt> NodeSubstituter::Substitute(SP<Stmt> &tree, SP<Expr> &node, SP<Expr> &val)
{
    if (tree == nullptr)
        return tree;

    switch (tree->kind)
    {
    case StmtKind::EXPR_STMT:
    {
        SP<ExprStmt> es = COPY(ExprStmt, tree);
        es->exp = Substitute(es->exp, node, val);
        return es;
    }
    case StmtKind::DECLARED_VAR:
    {
        SP<DeclaredVar> dv = COPY(DeclaredVar, tree);

        if (node->kind == ExprKind::VAR_REFERENCE &&
            val->kind == ExprKind::VAR_REFERENCE)
        {
            SP<VarReference> v_node = std::dynamic_pointer_cast<VarReference>(node);
            SP<VarReference> v_val = std::dynamic_pointer_cast<VarReference>(val);
            if (dv->name == v_node->name)
                dv->name = v_val->name;
        }

        dv->value = Substitute(dv->value, node, val);
        return dv;
    }
    case StmtKind::BLOCK:
    {
        SP<Block> b = COPY(Block, tree);
        for (auto &stmt : b->stmts)
            stmt = Substitute(stmt, node, val);
        return b;
    }
    case StmtKind::IF_STMT:
    {
        SP<IfStmt> i = COPY(IfStmt, tree);
        i->cond = Substitute(i->cond, node, val);
        i->then_branch = Substitute(i->then_branch, node, val);
        i->else_branch = Substitute(i->else_branch, node, val);
        return i;
    }
    case StmtKind::WHILE_STMT:
    {
        SP<WhileStmt> w = COPY(WhileStmt, tree);
        w->cond = Substitute(w->cond, node, val);
        w->body = Substitute(w->body, node, val);
        return w;
    }
    case StmtKind::FUNC_DECL:
    {
        SP<FuncDecl> fd = COPY(FuncDecl, tree);
        for (auto &stmt : fd->body)
            stmt = Substitute(stmt, node, val);
        return fd;
    }
    case StmtKind::RETURN:
    {
        SP<Return> r = COPY(Return, tree);
        r->ret_val = Substitute(r->ret_val, node, val);
        return r;
    }
    case StmtKind::THROW:
    {
        SP<Throw> t = COPY(Throw, tree);
        t->exp = Substitute(t->exp, node, val);
        return t;
    }
    case StmtKind::TRY_CATCH:
    {
        SP<TryCatch> tc = COPY(TryCatch, tree);
        tc->try_clause = Substitute(tc->try_clause, node, val);
        tc->catch_clause = Substitute(tc->catch_clause, node, val);
        return tc;
    }
    default:
    {
        return tree;
    }
    }
}