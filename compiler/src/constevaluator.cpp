#include "constevaluator.h"

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantEvaluator::EvaluateLiteral(Literal *l, bool &didSimp)
{
    return std::make_shared<Literal>(l->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateUnary(Unary *u, bool &didSimp)
{
    std::shared_ptr<Expr> rSimp = u->right->Evaluate(didSimp);
    Literal *r = dynamic_cast<Literal *>(rSimp.get());
    didSimp = r != nullptr;

    TokenID op = u->op.type;

    if (op == TokenID::MINUS && didSimp)
        return UNARY_MINUS(r);
    else if (op == TokenID::BANG && didSimp)
        return UNARY_BANG(r);
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBinary(Binary *b, bool &didSimp)
{
    std::shared_ptr<Expr> lSimp = b->left->Evaluate(didSimp);
    std::shared_ptr<Expr> rSimp = b->right->Evaluate(didSimp);

    Literal *l = dynamic_cast<Literal *>(lSimp.get());
    Literal *r = dynamic_cast<Literal *>(rSimp.get());

    didSimp = (l != nullptr) && (r != nullptr);
    TokenID op = b->op.type;

    if (op == TokenID::PLUS && didSimp)
        return BINARY_PLUS(l, r);
    else if (op == TokenID::MINUS && didSimp)
        return BINARY_MINUS(l, r);
    else if (op == TokenID::STAR && didSimp)
        return BINARY_STAR(l, r);
    else if (op == TokenID::SLASH && didSimp)
        return BINARY_SLASH(l, r);
    else if (op == TokenID::GT && didSimp)
        return BINARY_GT(l, r);
    else if (op == TokenID::LT && didSimp)
        return BINARY_LT(l, r);
    else if (op == TokenID::GEQ && didSimp)
        return BINARY_GEQ(l, r);
    else if (op == TokenID::LEQ && didSimp)
        return BINARY_LEQ(l, r);
    else if (op == TokenID::EQ_EQ && didSimp)
        return BINARY_EQ_EQ(l, r);
    else if (op == TokenID::BANG_EQ && didSimp)
        return BINARY_BANG_EQ(l, r);
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateAssign(Assign *a, bool &didSimp)
{
    std::shared_ptr<Expr> valSimp = a->val->Evaluate(didSimp);
    if (valSimp != nullptr)
    {
        didSimp = true;
        return std::make_shared<Assign>(a->target, a->val, a->Loc());
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateVarReference(VarReference *, bool &)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFunctionCall(FunctionCall *fc, bool &didSimp)
{
    for (size_t i = 0; i < fc->args.size(); i++)
    {
        std::shared_ptr<Expr> simp = fc->args[i]->Evaluate(didSimp);
        if (simp != nullptr)
        {
            didSimp = true;
            fc->args[i] = simp;
        }
    }
    if (didSimp)
        return std::make_shared<FunctionCall>(fc->name, fc->args, fc->Loc());
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateArrayIndex(ArrayIndex *ai, bool &didSimp)
{
    std::shared_ptr<Expr> iSimp = ai->index->Evaluate(didSimp);
    if (iSimp != nullptr)
    {
        didSimp = true;
        return std::make_shared<ArrayIndex>(ai->name, iSimp, ai->Loc());
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBracedInitialiser(BracedInitialiser *ia, bool &didSimp)
{
    for (size_t i = 0; i < ia->init.size(); i++)
    {
        std::shared_ptr<Expr> res = ia->init[i]->Evaluate(didSimp);
        if (res != nullptr)
        {
            didSimp = true;
            ia->init[i] = res;
        }
    }
    if (didSimp)
        return std::make_shared<BracedInitialiser>(ia->size, ia->init, ia->Loc());
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateDynamicAllocArray(DynamicAllocArray *da, bool &didSimp)
{
    std::shared_ptr<Expr> simp = da->size->Evaluate(didSimp);
    if (simp != nullptr)
    {
        didSimp = true;
        return std::make_shared<DynamicAllocArray>(da->GetType(), simp, da->Loc());
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFieldAccess(FieldAccess *, bool &)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateTypeCast(TypeCast *gf, bool &didSimp)
{
    std::shared_ptr<Expr> simp = gf->arg->Evaluate(didSimp);
    if (simp != nullptr)
    {
        didSimp = true;
        return std::make_shared<TypeCast>(gf->GetType(), simp, gf->Loc());
    }
    else
        return nullptr;
}

//------------------STATEMENTS---------------------//

void ConstantEvaluator::EvaluateExprStmt(ExprStmt *es, bool &didSimp)
{
    std::shared_ptr<Expr> exp = es->exp->Evaluate(didSimp);
    if (exp != nullptr)
        es->exp = exp;
}

void ConstantEvaluator::EvaluateDeclaredVar(DeclaredVar *dv, bool &didSimp)
{
    if (dv->value != nullptr)
    {
        std::shared_ptr<Expr> simp = dv->value->Evaluate(didSimp);
        if (simp != nullptr)
        {
            dv->value = simp;
            dv->value->t = dv->t;
        }
    }
}

void ConstantEvaluator::EvaluateBlock(Block *block, bool &didSimp)
{
    for (auto &stmt : block->stmts)
        stmt->Evaluate(didSimp);
}

void ConstantEvaluator::EvaluateIfStmt(IfStmt *i, bool &didSimp)
{
    std::shared_ptr<Expr> condSimp = i->cond->Evaluate(didSimp);
    if (condSimp != nullptr)
        i->cond = condSimp;

    i->thenBranch->Evaluate(didSimp);

    if (i->elseBranch != nullptr)
        i->elseBranch->Evaluate(didSimp);
}

void ConstantEvaluator::EvaluateWhileStmt(WhileStmt *ws, bool &didSimp)
{
    std::shared_ptr<Expr> condSimp = ws->cond->Evaluate(didSimp);
    if (condSimp != nullptr)
        ws->cond = condSimp;

    ws->body->Evaluate(didSimp);
}

void ConstantEvaluator::EvaluateFuncDecl(FuncDecl *fd, bool &didSimp)
{
    for (auto &stmt : fd->body)
        stmt->Evaluate(didSimp);
}

void ConstantEvaluator::EvaluateReturn(Return *r, bool &didSimp)
{
    std::shared_ptr<Expr> simp = r->retVal->Evaluate(didSimp);
    if (simp != nullptr)
        r->retVal = simp;
}

void ConstantEvaluator::EvaluateStructDecl(StructDecl *sd, bool &didSimp)
{
    for (auto &stmt : sd->decls)
        stmt->Evaluate(didSimp);
}

void ConstantEvaluator::EvaluateImportStmt(ImportStmt *, bool &didSimp)
{
}

void ConstantEvaluator::EvaluateThrow(Throw *t, bool &didSimp)
{
    std::shared_ptr<Expr> simp = t->exp->Evaluate(didSimp);
    if (simp != nullptr)
        t->exp = simp;
}

void ConstantEvaluator::EvaluateTryCatch(TryCatch *tc, bool &didSimp)
{
    tc->tryClause->Evaluate(didSimp);
    tc->catchClause->Evaluate(didSimp);
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> Literal::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateLiteral(this, didSimp);
}

std::shared_ptr<Expr> Unary::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateUnary(this, didSimp);
}

std::shared_ptr<Expr> Binary::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateBinary(this, didSimp);
}

std::shared_ptr<Expr> Assign::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateAssign(this, didSimp);
}

std::shared_ptr<Expr> VarReference::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateVarReference(this, didSimp);
}

std::shared_ptr<Expr> FunctionCall::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateFunctionCall(this, didSimp);
}

std::shared_ptr<Expr> ArrayIndex::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateArrayIndex(this, didSimp);
}

std::shared_ptr<Expr> BracedInitialiser::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateBracedInitialiser(this, didSimp);
}

std::shared_ptr<Expr> DynamicAllocArray::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateDynamicAllocArray(this, didSimp);
}

std::shared_ptr<Expr> FieldAccess::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateFieldAccess(this, didSimp);
}

std::shared_ptr<Expr> TypeCast::Evaluate(bool &didSimp)
{
    return ConstantEvaluator::EvaluateTypeCast(this, didSimp);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateExprStmt(this, didSimp);
}

void DeclaredVar::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateDeclaredVar(this, didSimp);
}

void Block::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateBlock(this, didSimp);
}

void IfStmt::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateIfStmt(this, didSimp);
}

void WhileStmt::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateWhileStmt(this, didSimp);
}

void FuncDecl::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateFuncDecl(this, didSimp);
}

void Return::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateReturn(this, didSimp);
}

void StructDecl::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateStructDecl(this, didSimp);
}

void ImportStmt::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateImportStmt(this, didSimp);
}

void Break::Evaluate(bool &didSimp)
{
}

void Throw::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateThrow(this, didSimp);
}

void TryCatch::Evaluate(bool &didSimp)
{
    ConstantEvaluator::EvaluateTryCatch(this, didSimp);
}