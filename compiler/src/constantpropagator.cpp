#include "constantpropagator.h"

bool IsConstant(std::shared_ptr<Expr> e)
{
    return dynamic_cast<Literal *>(e.get()) != nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::GetVariableValue(TypeData type, std::string name)
{
    for (int i = stack.size() - 1; i >= 0; i--)
    {
        VariableValue vv = stack[i];
        if (type == vv.type && name == vv.name)
            return vv.value;
    }
    return nullptr;
}

void ConstantPropagator::ClearCurrentDepth()
{
    while (stack.size() > 0 && stack.back().depth == depth)
        stack.pop_back();

    if (stack.size() == 1 && stack[0].depth == depth)
        stack.clear();
}

void ConstantPropagator::SetVariableValue(TypeData type, std::string name, std::shared_ptr<Expr> val)
{
    for (size_t i = stack.size() - 1; i >= 0; i--)
    {
        VariableValue vv = stack[i];
        if (type == vv.type && name == vv.name)
        {
            vv.value = val;
            break;
        }
    }
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantPropagator::PropagateUnary(Unary *u, bool &didSimp)
{
    std::shared_ptr<Expr> r = u->right->Propagate(*this, didSimp);
    if (r != nullptr && IsConstant(r))
    {
        didSimp = true;
        u->right = r;
    }

    if (didSimp)
    {
        std::shared_ptr<Expr> res = std::make_shared<Unary>(u->Loc(), u->right);
        res->t = u->GetType();
        return res;
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateBinary(Binary *b, bool &didSimp)
{
    std::shared_ptr<Expr> l = b->left->Propagate(*this, didSimp);
    if (l != nullptr && IsConstant(l))
    {
        didSimp = true;
        b->left = l;
    }

    std::shared_ptr<Expr> r = b->right->Propagate(*this, didSimp);
    if (r != nullptr && IsConstant(r))
    {
        didSimp = true;
        b->right = r;
    }

    if (didSimp)
    {
        std::shared_ptr<Expr> res = std::make_shared<Binary>(b->left, b->Loc(), b->right);
        res->t = b->GetType();
        return res;
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateAssign(Assign *a, bool &didSimp)
{
    std::shared_ptr<Expr> targetSimp = a->target->Evaluate(didSimp);
    std::shared_ptr<Expr> valSimp = a->val->Evaluate(didSimp);

    bool canReplace = targetSimp != nullptr;
    bool isValSimpd = valSimp != nullptr;
    if (!isValSimpd)
    {
        valSimp = std::dynamic_pointer_cast<Literal>(valSimp);
        isValSimpd = valSimp.get() != nullptr;
    }
    didSimp = targetSimp != nullptr;

    VarReference *targetAsVR = canReplace ? dynamic_cast<VarReference *>(targetSimp.get()) : dynamic_cast<VarReference *>(a->target.get());
    if (targetAsVR != nullptr)
    {
        if (isValSimpd)
            SetVariableValue(targetAsVR->GetType(), targetAsVR->name, valSimp);
        else
            SetVariableValue(targetAsVR->GetType(), targetAsVR->name, nullptr);
    }

    if (canReplace && isValSimpd)
        return std::make_shared<Assign>(targetSimp, valSimp, a->Loc());
    else if (canReplace && !isValSimpd)
        return std::make_shared<Assign>(targetSimp, a->val, a->Loc());
    else if (!canReplace && isValSimpd)
        return std::make_shared<Assign>(a->target, valSimp, a->Loc());
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateVarReference(VarReference *vr, bool &didSimp)
{
    std::shared_ptr<Expr> simp = GetVariableValue(vr->GetType(), vr->name);
    if (simp != nullptr && IsConstant(simp))
    {
        didSimp = true;
        simp->t = vr->GetType();
    }
    return simp;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateFunctionCall(FunctionCall *, bool &)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateArrayIndex(ArrayIndex *ai, bool &didSimp)
{
    TypeData arrayType = ai->GetType();
    arrayType.isArray++;

    std::shared_ptr<Expr> indexSimp = ai->index->Propagate(*this, didSimp);
    if (indexSimp != nullptr && IsConstant(indexSimp))
    {
        didSimp = true;
        ai->index = indexSimp;
    }

    std::shared_ptr<Expr> nameSimp = ai->name->Propagate(*this, didSimp);
    if (nameSimp != nullptr && IsConstant(nameSimp))
    {
        didSimp = true;
        ai->name = nameSimp;
    }

    if (didSimp)
    {
        std::shared_ptr<Expr> res = std::make_shared<ArrayIndex>(ai->name, ai->index, ai->Loc());
        res->t = ai->GetType();
        return res;
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateBracedInitialiser(BracedInitialiser *ia, bool &didSimp)
{
    for (size_t i = 0; i < ia->init.size(); i++)
    {
        std::shared_ptr<Expr> simp = ia->init[i]->Propagate(*this, didSimp);
        if (simp != nullptr && IsConstant(simp))
        {
            didSimp = true;
            ia->init[i] = simp;
        }
    }

    if (didSimp)
    {
        std::shared_ptr<Expr> res = std::make_shared<BracedInitialiser>(ia->init.size(), ia->init, ia->Loc());
        res->t = ia->GetType();
        return res;
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateDynamicAllocArray(DynamicAllocArray *da, bool &didSimp)
{
    std::shared_ptr<Expr> sizeSimp = da->size->Propagate(*this, didSimp);
    if (IsConstant(sizeSimp))
    {
        didSimp = true;
        std::shared_ptr<Expr> res = std::make_shared<DynamicAllocArray>(da->t, sizeSimp, da->Loc());
        res->t = da->GetType();
        return res;
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateFieldAccess(FieldAccess *fa, bool &didSimp)
{
    std::shared_ptr<Expr> accessorSimp = fa->accessor->Propagate(*this, didSimp);
    if (accessorSimp != nullptr && IsConstant(accessorSimp))
    {
        didSimp = true;
        std::shared_ptr<Expr> res = std::make_shared<FieldAccess>(accessorSimp, fa->accessee, fa->Loc());
        res->t = fa->GetType();
        return res;
    }
    else
        return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateTypeCast(TypeCast *gf, bool &didSimp)
{
    std::shared_ptr<Expr> valueSimp = gf->arg->Propagate(*this, didSimp);
    if (valueSimp != nullptr && IsConstant(valueSimp))
    {
        didSimp = true;
        std::shared_ptr<Expr> res = std::make_shared<TypeCast>(gf->GetType(), valueSimp, gf->Loc());
        res->t = gf->GetType();
        return res;
    }
    else
        return nullptr;
}

//------------------STATEMENT--------------------//

void ConstantPropagator::PropagateExprStmt(ExprStmt *es, bool &didSimp)
{
    std::shared_ptr<Expr> expSimp = es->exp->Propagate(*this, didSimp);

    if (expSimp != nullptr)
        es->exp = expSimp;
}

void ConstantPropagator::PropagateDeclaredVar(DeclaredVar *v, bool &didSimp)
{
    if (v->value != nullptr)
    {
        std::shared_ptr<Expr> valSimp = v->value->Propagate(*this, didSimp);
        if (valSimp != nullptr)
            v->value = valSimp;

        stack.push_back(VariableValue(depth, v->t, v->name, v->value));
    }
}

void ConstantPropagator::PropagateBlock(Block *b, bool &didSimp)
{
    depth++;
    for (auto &stmt : b->stmts)
        stmt->Propagate(*this, didSimp);

    ClearCurrentDepth();
    depth--;
}

void ConstantPropagator::PropagateIfStmt(IfStmt *i, bool &didSimp)
{
    std::shared_ptr<Expr> condSimp = i->cond->Propagate(*this, didSimp);
    if (condSimp != nullptr)
        i->cond = condSimp;

    i->thenBranch->Propagate(*this, didSimp);
    i->elseBranch->Propagate(*this, didSimp);
}

void ConstantPropagator::PropagateWhileStmt(WhileStmt *ws, bool &didSimp)
{
    std::shared_ptr<Expr> condSimp = ws->cond->Propagate(*this, didSimp);
    if (condSimp != nullptr)
        ws->cond = condSimp;

    ws->body->Propagate(*this, didSimp);
}

void ConstantPropagator::PropagateFuncDecl(FuncDecl *fd, bool &didSimp)
{
    depth++;

    for (auto &stmt : fd->body)
        stmt->Propagate(*this, didSimp);

    ClearCurrentDepth();
    depth--;
}

void ConstantPropagator::PropagateReturn(Return *r, bool &didSimp)
{
    std::shared_ptr<Expr> retValSimp = r->retVal->Propagate(*this, didSimp);
    if (retValSimp != nullptr)
        r->retVal = retValSimp;
}

void ConstantPropagator::PropagateStructDecl(StructDecl *sd, bool &didSimp)
{
    for (auto decl : sd->decls)
        decl->Propagate(*this, didSimp);
}

void ConstantPropagator::PropagateImportStmt(ImportStmt *, bool &)
{
}

void ConstantPropagator::PropagateThrow(Throw *t, bool &didSimp)
{
    std::shared_ptr<Expr> throwSimp = t->exp->Propagate(*this, didSimp);
    if (throwSimp != nullptr)
        t->exp = throwSimp;
}

void ConstantPropagator::PropagateTryCatch(TryCatch *tc, bool &didSimp)
{
    tc->tryClause->Propagate(*this, didSimp);
    tc->catchClause->Propagate(*this, didSimp);
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> Literal::Propagate(ConstantPropagator &, bool &)
{
    return nullptr;
}

std::shared_ptr<Expr> Unary::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateUnary(this, didSimp);
}

std::shared_ptr<Expr> Binary::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateBinary(this, didSimp);
}

std::shared_ptr<Expr> Assign::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateAssign(this, didSimp);
}

std::shared_ptr<Expr> VarReference::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateVarReference(this, didSimp);
}

std::shared_ptr<Expr> FunctionCall::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateFunctionCall(this, didSimp);
}

std::shared_ptr<Expr> ArrayIndex::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateArrayIndex(this, didSimp);
}

std::shared_ptr<Expr> BracedInitialiser::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateBracedInitialiser(this, didSimp);
}

std::shared_ptr<Expr> DynamicAllocArray::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateDynamicAllocArray(this, didSimp);
}

std::shared_ptr<Expr> FieldAccess::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateFieldAccess(this, didSimp);
}

std::shared_ptr<Expr> TypeCast::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    return cp.PropagateTypeCast(this, didSimp);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateExprStmt(this, didSimp);
}

void DeclaredVar::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateDeclaredVar(this, didSimp);
}

void Block::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateBlock(this, didSimp);
}

void IfStmt::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateIfStmt(this, didSimp);
}

void WhileStmt::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateWhileStmt(this, didSimp);
}

void FuncDecl::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateFuncDecl(this, didSimp);
}

void Return::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateReturn(this, didSimp);
}

void StructDecl::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateStructDecl(this, didSimp);
}

void ImportStmt::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateImportStmt(this, didSimp);
}

void Break::Propagate(ConstantPropagator &, bool &)
{
}

void Throw::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateThrow(this, didSimp);
}

void TryCatch::Propagate(ConstantPropagator &cp, bool &didSimp)
{
    cp.PropagateTryCatch(this, didSimp);
}