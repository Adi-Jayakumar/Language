#include "constevaluator.h"

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantEvaluator::EvaluateLiteral(Literal *l)
{
    return std::make_shared<Literal>(l->Loc());
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateUnary(Unary *u)
{
    std::shared_ptr<Expr> rSimp = u->right->Evaluate();
    Literal *r = dynamic_cast<Literal *>(rSimp.get());
    bool didSimp = r != nullptr;

    TokenID op = u->op.type;

    if (op == TokenID::MINUS && didSimp)
        return UNARY_MINUS(r);
    else if (op == TokenID::BANG && didSimp)
        return UNARY_BANG(r);
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBinary(Binary *b)
{
    std::shared_ptr<Expr> lSimp = b->left->Evaluate();
    std::shared_ptr<Expr> rSimp = b->right->Evaluate();

    Literal *l = dynamic_cast<Literal *>(lSimp.get());
    Literal *r = dynamic_cast<Literal *>(rSimp.get());

    bool didSimp = (l != nullptr) && (r != nullptr);
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

std::shared_ptr<Expr> ConstantEvaluator::EvaluateAssign(Assign *a)
{
    std::shared_ptr<Expr> valSimp = a->val->Evaluate();
    if (valSimp != nullptr)
        return std::make_shared<Assign>(a->target, a->val, a->Loc());
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateVarReference(VarReference *vr)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFunctionCall(FunctionCall *fc)
{
    bool didSimp = false;
    for (size_t i = 0; i < fc->args.size(); i++)
    {
        std::shared_ptr<Expr> simp = fc->args[i]->Evaluate();
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

std::shared_ptr<Expr> ConstantEvaluator::EvaluateArrayIndex(ArrayIndex *ai)
{
    std::shared_ptr<Expr> iSimp = ai->index->Evaluate();
    if (iSimp != nullptr)
        return std::make_shared<ArrayIndex>(ai->name, iSimp, ai->Loc());
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBracedInitialiser(BracedInitialiser *ia)
{
    bool didSimp = false;
    for (size_t i = 0; i < ia->init.size(); i++)
    {
        std::shared_ptr<Expr> res = ia->init[i]->Evaluate();
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

std::shared_ptr<Expr> ConstantEvaluator::EvaluateDynamicAllocArray(DynamicAllocArray *da)
{
    std::shared_ptr<Expr> simp = da->size->Evaluate();
    if (simp != nullptr)
        return std::make_shared<DynamicAllocArray>(da->GetType(), simp, da->Loc());
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFieldAccess(FieldAccess *fa)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateTypeCast(TypeCast *gf)
{
    std::shared_ptr<Expr> simp = gf->arg->Evaluate();
    if (simp != nullptr)
        return std::make_shared<TypeCast>(gf->GetType(), simp, gf->Loc());
    else
        return nullptr;
}

//------------------STATEMENTS---------------------//

void ConstantEvaluator::EvaluateExprStmt(ExprStmt *es)
{
    std::shared_ptr<Expr> exp = es->exp->Evaluate();
    if (exp != nullptr)
        es->exp = exp;
}

void ConstantEvaluator::EvaluateDeclaredVar(DeclaredVar *dv)
{
    if (dv->value != nullptr)
    {
        std::cout << "RRUNNING DECLARED VAR" << std::endl;
        std::shared_ptr<Expr> simp = dv->value->Evaluate();
        if (simp != nullptr)
        {
            dv->value = simp;
            dv->value->t = dv->t;
        }
    }
}

void ConstantEvaluator::EvaluateBlock(Block *block)
{
    for (auto &stmt : block->stmts)
        stmt->Evaluate();
}

void ConstantEvaluator::EvaluateIfStmt(IfStmt *i)
{
    std::shared_ptr<Expr> condSimp = i->cond->Evaluate();
    if (condSimp != nullptr)
        i->cond = condSimp;

    i->thenBranch->Evaluate();

    if (i->elseBranch != nullptr)
        i->elseBranch->Evaluate();
}

void ConstantEvaluator::EvaluateWhileStmt(WhileStmt *ws)
{
    std::shared_ptr<Expr> condSimp = ws->cond->Evaluate();
    if (condSimp != nullptr)
        ws->cond = condSimp;

    ws->body->Evaluate();
}

void ConstantEvaluator::EvaluateFuncDecl(FuncDecl *fd)
{
    for (auto &stmt : fd->body)
        stmt->Evaluate();
}

void ConstantEvaluator::EvaluateReturn(Return *r)
{
    std::shared_ptr<Expr> simp = r->retVal->Evaluate();
    if (simp != nullptr)
        r->retVal = simp;
}

void ConstantEvaluator::EvaluateStructDecl(StructDecl *sd)
{
    for (auto &stmt : sd->decls)
    {
        stmt->Evaluate();
    }
}

void ConstantEvaluator::EvaluateImportStmt(ImportStmt *)
{
}

void ConstantEvaluator::EvaluateThrow(Throw *t)
{
    std::shared_ptr<Expr> simp = t->exp->Evaluate();
    if (simp != nullptr)
        t->exp = simp;
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