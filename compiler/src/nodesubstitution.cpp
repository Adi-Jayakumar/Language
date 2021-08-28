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

SP<Stmt> NodeSubstitution::Substitute(SP<Stmt> &tree, SP<Stmt> &node, SP<Stmt> &val)
{
}