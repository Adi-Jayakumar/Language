#include "nodeequal.h"

#define GET_NODE_KIND(kind)                             \
    SP<kind> l = std::dynamic_pointer_cast<kind>(left); \
    SP<kind> r = std::dynamic_pointer_cast<kind>(right)

bool NodeEqual::Equal(const SP<Expr> &left, const SP<Expr> &right)
{
    if (left == nullptr && right == nullptr)
        return true;

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
        return l->op.type == r->op.type &&
               Equal(l->right, r->right);
    }
    case ExprKind::BINARY:
    {
        GET_NODE_KIND(Binary);
        return l->op.type == r->op.type &&
               Equal(l->left, r->left) &&
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
        return Equal(l->target, r->target) &&
               Equal(l->val, r->val);
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
        return Equal(l->name, r->name) &&
               Equal(l->index, r->index);
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
        return l->t == r->t &&
               Equal(l->size, r->size);
    }
    case ExprKind::FIELD_ACCESS:
    {
        GET_NODE_KIND(FieldAccess);
        return Equal(l->accessor, r->accessor) &&
               Equal(l->accessee, r->accessee);
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

bool NodeEqual::Equal(const SP<Stmt> &left, const SP<Stmt> &right)
{
    if (left->kind != right->kind)
        return false;

    switch (left->kind)
    {
    case StmtKind::EXPR_STMT:
    {
        GET_NODE_KIND(ExprStmt);
        return Equal(l->exp, r->exp);
    }
    case StmtKind::DECLARED_VAR:
    {
        GET_NODE_KIND(DeclaredVar);
        return l->t == r->t &&
               l->name == r->name &&
               Equal(l->value, r->value);
    }
    case StmtKind::BLOCK:
    {
        GET_NODE_KIND(Block);

        for (size_t i = 0; i < l->stmts.size(); i++)
        {
            if (!Equal(l->stmts[i], r->stmts[i]))
                return false;
        }
        return true;
    }
    case StmtKind::IF_STMT:
    {
        GET_NODE_KIND(IfStmt);
        return Equal(l->cond, r->cond) &&
               Equal(l->thenBranch, r->thenBranch) &&
               Equal(l->elseBranch, r->elseBranch);
    }
    case StmtKind::WHILE_STMT:
    {
        GET_NODE_KIND(WhileStmt);
        return Equal(l->cond, r->cond) &&
               Equal(l->body, r->body);
    }
    case StmtKind::FUNC_DECL:
    {
        GET_NODE_KIND(FuncDecl);

        if (l->ret != r->ret)
            return false;

        if (l->name != r->name)
            return false;

        if (l->params.size() != r->params.size())
            return false;

        for (size_t i = 0; i < l->params.size(); i++)
        {
            if (l->params[i].first != r->params[i].first &&
                l->params[i].second != r->params[i].second)
                return false;
        }

        if (l->preConds.size() != r->preConds.size())
            return false;

        for (size_t i = 0; i < l->preConds.size(); i++)
        {
            if (!Equal(l->preConds[i], r->preConds[i]))
                return false;
        }

        if (l->body.size() != r->body.size())
            return false;

        for (size_t i = 0; i < l->body.size(); i++)
        {
            if (!Equal(l->body[i], r->body[i]))
                return false;
        }

        return true;
    }
    case StmtKind::RETURN:
    {
        GET_NODE_KIND(Return);
        return Equal(l->retVal, r->retVal);
    }
    case StmtKind::STRUCT_DECL:
    {
        GET_NODE_KIND(StructDecl);
        if (l->name != r->name ||
            l->parent != r->parent ||
            l->decls.size() != r->decls.size())
            return false;

        for (size_t i = 0; i < l->decls.size(); i++)
        {
            if (!Equal(l->decls[i], r->decls[i]))
                return false;
        }
        return true;
    }
    case StmtKind::IMPORT_STMT:
    {
        GET_NODE_KIND(ImportStmt);
        if (l->libraries.size() != r->libraries.size())
            return false;

        for (size_t i = 0; i < l->libraries.size(); i++)
        {
            if (l->libraries[i] != r->libraries[i])
                return false;
        }
        return true;
    }
    case StmtKind::BREAK:
    {
        return true;
    }
    case StmtKind::THROW:
    {
        GET_NODE_KIND(Throw);
        return Equal(l->exp, r->exp);
    }
    case StmtKind::TRY_CATCH:
    {
        GET_NODE_KIND(TryCatch);
        if (l->catchVar.first != r->catchVar.first ||
            l->catchVar.second != r->catchVar.second)
            return false;

        return Equal(l->tryClause, r->tryClause) &&
               Equal(l->catchClause, r->catchClause);
    }
    }
    return false;
}