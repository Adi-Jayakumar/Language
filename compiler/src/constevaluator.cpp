#include "constevaluator.h"

bool IsLiteral(std::shared_ptr<Expr> exp)
{
    return std::dynamic_pointer_cast<Literal>(exp) != nullptr;
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantEvaluator::EvaluateLiteral(Literal *l)
{
    return std::make_shared<Literal>(*l);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateUnary(Unary *u)
{
    u->right = u->right->Evaluate();
    bool isLit = IsLiteral(u->right);
    TokenID op = u->op.type;

    Literal *r = dynamic_cast<Literal *>(u->right.get());

    if (op == TokenID::MINUS && isLit)
        return UNARY_MINUS(r);
    else if (op == TokenID::BANG && isLit)
        return UNARY_BANG(r);
    else
        return std::make_shared<Unary>(*u);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBinary(Binary *b)
{
    b->left = b->left->Evaluate();
    bool isLit = IsLiteral(b->left);

    b->right = b->right->Evaluate();
    isLit = isLit && IsLiteral(b->right);

    Literal *l = dynamic_cast<Literal *>(b->left.get());
    Literal *r = dynamic_cast<Literal *>(b->right.get());

    TokenID op = b->op.type;

    if (op == TokenID::PLUS && isLit)
        return BINARY_PLUS(l, r);
    else if (op == TokenID::MINUS && isLit)
        return BINARY_MINUS(l, r);
    else if (op == TokenID::STAR && isLit)
        return BINARY_STAR(l, r);
    else if (op == TokenID::SLASH && isLit)
        return BINARY_SLASH(l, r);
    else if (op == TokenID::GT && isLit)
        return BINARY_GT(l, r);
    else if (op == TokenID::LT && isLit)
        return BINARY_LT(l, r);
    else if (op == TokenID::GEQ && isLit)
        return BINARY_GEQ(l, r);
    else if (op == TokenID::LEQ && isLit)
        return BINARY_LEQ(l, r);
    else if (op == TokenID::EQ_EQ && isLit)
        return BINARY_EQ_EQ(l, r);
    else if (op == TokenID::BANG_EQ && isLit)
        return BINARY_BANG_EQ(l, r);
    else
        return std::make_shared<Binary>(*b);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateAssign(Assign *a)
{
    a->val = a->val->Evaluate();
    a->target = a->target->Evaluate();
    return std::make_shared<Assign>(*a);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateVarReference(VarReference *vr)
{
    return std::make_shared<VarReference>(*vr);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFunctionCall(FunctionCall *fc)
{
    return std::make_shared<FunctionCall>(*fc);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateArrayIndex(ArrayIndex *ai)
{
    ai->index = ai->index->Evaluate();
    return std::make_shared<ArrayIndex>(*ai);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateBracedInitialiser(BracedInitialiser *ia)
{
    for (size_t i = 0; i < ia->init.size(); i++)
        ia->init[i] = ia->init[i]->Evaluate();

    return std::make_shared<BracedInitialiser>(*ia);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateDynamicAllocArray(DynamicAllocArray *da)
{
    da->size = da->size->Evaluate();
    return std::make_shared<DynamicAllocArray>(*da);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateFieldAccess(FieldAccess *fa)
{
    return std::make_shared<FieldAccess>(*fa);
}

std::shared_ptr<Expr> ConstantEvaluator::EvaluateTypeCast(TypeCast *gf)
{
    gf->arg = gf->arg->Evaluate();
    return std::make_shared<TypeCast>(*gf);
}

//------------------STATEMENTS---------------------//
void ConstantEvaluator::EvaluateExprStmt(ExprStmt *es)
{
    es->exp = es->exp->Evaluate();
}

void ConstantEvaluator::EvaluateDeclaredVar(DeclaredVar *dv)
{
    if (dv->value != nullptr)
        dv->value = dv->value->Evaluate();
}

void ConstantEvaluator::EvaluateBlock(Block *b)
{
    for (auto &stmt : b->stmts)
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
    for (auto &member : sd->decls)
        member->Evaluate();
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