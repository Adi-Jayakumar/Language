#include "constevaluator.h"

bool IsConstant(Expr *e)
{
    return dynamic_cast<Literal *>(e) != nullptr;
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantEvaluator::EvaluateLiteral(Literal *l)
{
    return std::make_shared<Literal>(l->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateUnary(Unary *u)
{
    std::shared_ptr<Expr> rSimp = u->right->Evaluate();
    Literal *r = dynamic_cast<Literal *>(rSimp.get());
    bool isSimp = r != nullptr;

    std::shared_ptr<Expr> result = std::make_shared<Unary>(u->op, u->right);
    result->t = u->t;

    TokenID op = u->op.type;

    if (op == TokenID::MINUS && isSimp)
        return UNARY_MINUS(r);
    else if (op == TokenID::BANG && isSimp)
        return UNARY_BANG(r);
    else
        return result;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBinary(Binary *b)
{
    std::shared_ptr<Expr> lSimp = b->left->Evaluate();
    std::shared_ptr<Expr> rSimp = b->right->Evaluate();

    Literal *l = dynamic_cast<Literal *>(lSimp.get());
    Literal *r = dynamic_cast<Literal *>(rSimp.get());

    bool isSimp = (l != nullptr) && (r != nullptr);
    TokenID op = b->op.type;

    std::shared_ptr<Expr> result = std::make_shared<Binary>(lSimp, b->op, rSimp);
    result->t = b->t;

    if (op == TokenID::PLUS && isSimp)
        return BINARY_PLUS(l, r);
    else if (op == TokenID::MINUS && isSimp)
        return BINARY_MINUS(l, r);
    else if (op == TokenID::STAR && isSimp)
        return BINARY_STAR(l, r);
    else if (op == TokenID::SLASH && isSimp)
        return BINARY_SLASH(l, r);
    else if (op == TokenID::GT && isSimp)
        return BINARY_GT(l, r);
    else if (op == TokenID::LT && isSimp)
        return BINARY_LT(l, r);
    else if (op == TokenID::GEQ && isSimp)
        return BINARY_GEQ(l, r);
    else if (op == TokenID::LEQ && isSimp)
        return BINARY_LEQ(l, r);
    else if (op == TokenID::EQ_EQ && isSimp)
        return BINARY_EQ_EQ(l, r);
    else if (op == TokenID::BANG_EQ && isSimp)
        return BINARY_BANG_EQ(l, r);
    else
        return result;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateAssign(Assign *a)
{
    std::shared_ptr<Expr> valSimp = a->val->Evaluate();
    return std::make_shared<Assign>(a->target, a->val, a->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateVarReference(VarReference *vr)
{
    return std::make_shared<VarReference>(vr->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFunctionCall(FunctionCall *fc)
{
    for (size_t i = 0; i < fc->args.size(); i++)
        fc->args[i] = fc->args[i]->Evaluate();
    return std::make_shared<FunctionCall>(fc->name, fc->args, fc->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateArrayIndex(ArrayIndex *ai)
{
    std::shared_ptr<Expr> iSimp = ai->index->Evaluate();
    return std::make_shared<ArrayIndex>(ai->name, ai->index, ai->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBracedInitialiser(BracedInitialiser *ia)
{
    for (size_t i = 0; i < ia->init.size(); i++)
        ia->init[i] = ia->init[i]->Evaluate();
    return std::make_shared<BracedInitialiser>(ia->size, ia->init, ia->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateDynamicAllocArray(DynamicAllocArray *da)
{
    da->size = da->size->Evaluate();
    return std::make_shared<DynamicAllocArray>(da->GetType(), da->size, da->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFieldAccess(FieldAccess *fa)
{
    return std::make_shared<FieldAccess>(fa->accessor, fa->accessee, fa->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateTypeCast(TypeCast *gf)
{
    gf->arg = gf->arg->Evaluate();
    return std::make_shared<TypeCast>(gf->GetType(), gf->arg, gf->Loc());
}

//------------------STATEMENTS---------------------//

void ConstantEvaluator::EvaluateExprStmt(ExprStmt *es)
{
    es->exp = es->exp->Evaluate();
}

void ConstantEvaluator::EvaluateDeclaredVar(DeclaredVar *dv)
{
    if (dv->value != nullptr)
    {
        dv->value = dv->value->Evaluate();
        dv->value->t = dv->t;
    }
}

void ConstantEvaluator::EvaluateBlock(Block *block)
{
    for (auto &stmt : block->stmts)
        stmt->Evaluate();
}

void ConstantEvaluator::EvaluateIfStmt(IfStmt *i)
{
    i->cond = i->cond->Evaluate();
    i->thenBranch->Evaluate();

    if (i->elseBranch != nullptr)
        i->elseBranch->Evaluate();
}

void ConstantEvaluator::EvaluateWhileStmt(WhileStmt *ws)
{
    ws->cond = ws->cond->Evaluate();
    ws->body->Evaluate();
}

void ConstantEvaluator::EvaluateFuncDecl(FuncDecl *fd)
{
    for (auto &stmt : fd->body)
        stmt->Evaluate();
}

void ConstantEvaluator::EvaluateReturn(Return *r)
{
    r->retVal = r->retVal->Evaluate();
}

void ConstantEvaluator::EvaluateStructDecl(StructDecl *sd)
{
    for (auto &stmt : sd->decls)
    {
        if (stmt != nullptr)
            stmt->Evaluate();
    }
}

void ConstantEvaluator::EvaluateImportStmt(ImportStmt *)
{
}

void ConstantEvaluator::EvaluateThrow(Throw *t)
{
    t->exp = t->exp->Evaluate();
}

void ConstantEvaluator::EvaluateTryCatch(TryCatch *tc)
{
    tc->tryClause->Evaluate();
    tc->catchClause->Evaluate();
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> Literal::Evaluate()
{
    return ConstantEvaluator::EvaluateLiteral(this);
}

std::shared_ptr<Expr> Unary::Evaluate()
{
    return ConstantEvaluator::EvaluateUnary(this);
}

std::shared_ptr<Expr> Binary::Evaluate()
{
    return ConstantEvaluator::EvaluateBinary(this);
}

std::shared_ptr<Expr> Assign::Evaluate()
{
    return ConstantEvaluator::EvaluateAssign(this);
}

std::shared_ptr<Expr> VarReference::Evaluate()
{
    return ConstantEvaluator::EvaluateVarReference(this);
}

std::shared_ptr<Expr> FunctionCall::Evaluate()
{
    return ConstantEvaluator::EvaluateFunctionCall(this);
}

std::shared_ptr<Expr> ArrayIndex::Evaluate()
{
    return ConstantEvaluator::EvaluateArrayIndex(this);
}

std::shared_ptr<Expr> BracedInitialiser::Evaluate()
{
    return ConstantEvaluator::EvaluateBracedInitialiser(this);
}

std::shared_ptr<Expr> DynamicAllocArray::Evaluate()
{
    return ConstantEvaluator::EvaluateDynamicAllocArray(this);
}

std::shared_ptr<Expr> FieldAccess::Evaluate()
{
    return ConstantEvaluator::EvaluateFieldAccess(this);
}

std::shared_ptr<Expr> TypeCast::Evaluate()
{
    return ConstantEvaluator::EvaluateTypeCast(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Evaluate()
{
    ConstantEvaluator::EvaluateExprStmt(this);
}

void DeclaredVar::Evaluate()
{
    ConstantEvaluator::EvaluateDeclaredVar(this);
}

void Block::Evaluate()
{
    ConstantEvaluator::EvaluateBlock(this);
}

void IfStmt::Evaluate()
{
    ConstantEvaluator::EvaluateIfStmt(this);
}

void WhileStmt::Evaluate()
{
    ConstantEvaluator::EvaluateWhileStmt(this);
}

void FuncDecl::Evaluate()
{
    ConstantEvaluator::EvaluateFuncDecl(this);
}

void Return::Evaluate()
{
    ConstantEvaluator::EvaluateReturn(this);
}

void StructDecl::Evaluate()
{
    ConstantEvaluator::EvaluateStructDecl(this);
}

void ImportStmt::Evaluate()
{
    ConstantEvaluator::EvaluateImportStmt(this);
}

void Break::Evaluate()
{
}

void Throw::Evaluate()
{
    ConstantEvaluator::EvaluateThrow(this);
}

void TryCatch::Evaluate()
{
    ConstantEvaluator::EvaluateTryCatch(this);
}