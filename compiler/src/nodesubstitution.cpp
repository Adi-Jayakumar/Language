#include "nodesubstitution.h"

SP<Expr> NodeSubstitution::Substitute(SP<Expr> &tree, SP<Expr> &node, SP<Expr> &val)
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
        SP<Unary> u = std::dynamic_pointer_cast<Unary>(tree);
        u->right = Substitute(u->right, node, val);
        return u;
    }
    case ExprKind::BINARY:
    {
        SP<Binary> b = std::dynamic_pointer_cast<Binary>(tree);
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
        SP<Assign> a = std::dynamic_pointer_cast<Assign>(tree);
        a->target = Substitute(a->target, node, val);
        a->val = Substitute(a->val, node, val);
        return a;
    }
    case ExprKind::FUNCTION_CALL:
    {
        SP<FunctionCall> fc = std::dynamic_pointer_cast<FunctionCall>(tree);
        for (auto &arg : fc->args)
            arg = Substitute(arg, node, val);
        return fc;
    }
    case ExprKind::ARRAY_INDEX:
    {
        SP<ArrayIndex> ai = std::dynamic_pointer_cast<ArrayIndex>(tree);
        ai->name = Substitute(ai->name, node, val);
        ai->index = Substitute(ai->index, node, val);
        return ai;
    }
    case ExprKind::BRACED_INITIALISER:
    {
        SP<BracedInitialiser> bi = std::dynamic_pointer_cast<BracedInitialiser>(tree);
        for (auto &init : bi->init)
            init = Substitute(init, node, val);
        return bi;
    }
    case ExprKind::DYNAMIC_ALLOC_ARRAY:
    {
        SP<DynamicAllocArray> da = std::dynamic_pointer_cast<DynamicAllocArray>(tree);
        da->size = Substitute(da->size, node, val);
        return da;
    }
    case ExprKind::FIELD_ACCESS:
    {
        SP<FieldAccess> fa = std::dynamic_pointer_cast<FieldAccess>(tree);
        fa->accessee = Substitute(fa->accessee, node, val);
        fa->accessor = Substitute(fa->accessor, node, val);
        return fa;
    }
    case ExprKind::TYPE_CAST:
    {
        SP<TypeCast> tc = std::dynamic_pointer_cast<TypeCast>(tree);
        tc->arg = Substitute(tc->arg, node, val);
        return tc;
    }
    case ExprKind::SEQUENCE:
    {
        SP<Sequence> s = std::dynamic_pointer_cast<Sequence>(tree);
        s->start = Substitute(s->start, node, val);
        s->step = Substitute(s->step, node, val);
        s->end = Substitute(s->end, node, val);
        s->term = Substitute(s->term, node, val);
        return s;
    }
    }
    return tree;
}

void NodeSubstitution::Substitute(SP<Stmt> &tree, SP<Expr> &node, SP<Expr> &val)
{
    if (tree == nullptr)
        return;

    switch (tree->kind)
    {
    case StmtKind::EXPR_STMT:
    {
        SP<ExprStmt> es = std::dynamic_pointer_cast<ExprStmt>(tree);
        es->exp = Substitute(es->exp, node, val);
        break;
    }
    case StmtKind::DECLARED_VAR:
    {
        SP<DeclaredVar> dv = std::dynamic_pointer_cast<DeclaredVar>(tree);
        dv->value = Substitute(dv->value, node, val);
        break;
    }
    case StmtKind::BLOCK:
    {
        SP<Block> b = std::dynamic_pointer_cast<Block>(tree);
        for (auto &stmt : b->stmts)
            Substitute(stmt, node, val);
        break;
    }
    case StmtKind::IF_STMT:
    {
        SP<IfStmt> i = std::dynamic_pointer_cast<IfStmt>(tree);
        i->cond = Substitute(i->cond, node, val);
        Substitute(i->thenBranch, node, val);
        Substitute(i->elseBranch, node, val);
        break;
    }
    case StmtKind::WHILE_STMT:
    {
        SP<WhileStmt> w = std::dynamic_pointer_cast<WhileStmt>(tree);
        w->cond = Substitute(w->cond, node, val);
        Substitute(w->body, node, val);
        break;
    }
    case StmtKind::FUNC_DECL:
    {
        SP<FuncDecl> fd = std::dynamic_pointer_cast<FuncDecl>(tree);
        for (auto &stmt : fd->body)
            Substitute(stmt, node, val);
        break;
    }
    case StmtKind::RETURN:
    {
        SP<Return> r = std::dynamic_pointer_cast<Return>(tree);
        r->retVal = (r->retVal, node, val);
        break;
    }
    case StmtKind::STRUCT_DECL:
    case StmtKind::IMPORT_STMT:
    case StmtKind::BREAK:
    {
        break;
    }
    case StmtKind::THROW:
    {
        SP<Throw> t = std::dynamic_pointer_cast<Throw>(tree);
        t->exp = Substitute(t->exp, node, val);
        break;
    }
    case StmtKind::TRY_CATCH:
    {
        SP<TryCatch> tc = std::dynamic_pointer_cast<TryCatch>(tree);
        Substitute(tc->tryClause, node, val);
        Substitute(tc->catchClause, node, val);
        break;
    }
    }
}