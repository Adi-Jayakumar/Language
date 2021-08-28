#include "nodeequal.h"

#define GET_NODE_KIND(kind)                             \
    SP<kind> l = std::dynamic_pointer_cast<kind>(left); \
    SP<kind> r = std::dynamic_pointer_cast<kind>(right)

bool NodeEqual::Equal(const SP<Expr> &left, const SP<Expr> &right)
{
    if (left->kind != right->kind)
        return false;

    switch (left->kind)
    {
    case ExprKind::LITERAL:
    {
        GET_NODE_KIND(Literal);
        return l->Loc().literal == r->Loc().literal;
    }
    case ExprKind::UNARY:
    {
        GET_NODE_KIND(Unary);
        return l->op.type == r->op.type && Equal(l->right, r->right);
    }
    case ExprKind::BINARY:
    {
        GET_NODE_KIND(Binary);

        return l->op.type == r->op.type &&
               Equal(l->left, r->right) &&
               Equal(l->right, r->right);
    }
    case ExprKind::VAR_REFERENCE:
    {
        GET_NODE_KIND(VarReference);
        return l->name == r->name;
    }
    case ExprKind::ASSIGN:
    {
        GET_NODE_KIND(Assign);
        return Equal(l->target, r->target) && Equal(l->val, r->val);
    }
    case ExprKind::FUNCTION_CALL:
    {
        GET_NODE_KIND(FunctionCall);

        if (l->args.size() != r->args.size())
            return false;

        for (size_t i = 0; i < l->args.size(); i++)
        {
            if (!Equal(l->args[i], r->args[i]))
                return false;
        }
        return true;
    }
    case ExprKind::ARRAY_INDEX:
    {
        GET_NODE_KIND(ArrayIndex);
        return Equal(l->name, r->name) && Equal(l->index, r->index);
    }
    case ExprKind::BRACED_INITIALISER:
    {
        GET_NODE_KIND(BracedInitialiser);

        if (l->size != r->size)
            return false;

        for (size_t i = 0; i < l->init.size(); i++)
        {
            if (!Equal(l->init[i], r->init[i]))
                return false;
        }
        return true;
    }
    case ExprKind::DYNAMIC_ALLOC_ARRAY:
    {
        GET_NODE_KIND(DynamicAllocArray);
        return l->t == r->t && Equal(l->size, r->size);
    }
    case ExprKind::FIELD_ACCESS:
    {
        GET_NODE_KIND(FieldAccess);
        return Equal(l->accessor, r->accessor) && Equal(l->accessee, r->accessee);
    }
    case ExprKind::TYPE_CAST:
    {
        GET_NODE_KIND(TypeCast);
        return l->type == r->type && Equal(l->arg, r->arg);
    }
    case ExprKind::SEQUENCE:
    {
        GET_NODE_KIND(Sequence);
        return Equal(l->start, r->start) &&
               Equal(l->step, r->step) &&
               Equal(l->end, r->end) &&
               Equal(l->var, r->var) &&
               Equal(l->term, r->term) &&
               l->op == r->op;
    }
    }
    return false;
}