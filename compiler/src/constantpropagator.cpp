#include "constantpropagator.h"

bool IsConstant(std::shared_ptr<Expr> e)
{
    return dynamic_cast<Literal *>(e.get()) != nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::GetVariableValue(TypeData type, std::string name)
{
    for (int i = stack.size() - 1; i >= 0; i--)
    {
        LiteralValue vv = stack[i];
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
    for (int i = stack.size() - 1; i >= 0; i--)
    {
        LiteralValue *vv = &stack[i];
        if (type == vv->type && name == vv->name)
        {
            vv->value = val;
            break;
        }
    }
}

void ConstantPropagator::SetArrayIndex(TypeData type, std::string name, size_t index, std::shared_ptr<Expr> value)
{
    std::shared_ptr<Expr> array = GetVariableValue(type, name);
    BracedInitialiser *bi = dynamic_cast<BracedInitialiser *>(array.get());
    bi->init[index] = value;
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantPropagator::PropagateLiteral(Literal *l)
{
    return std::make_shared<Literal>(*l);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateUnary(Unary *u)
{
    std::shared_ptr<Expr> rSimp = u->right->Propagate(*this);
    std::shared_ptr<Unary> copy = std::make_shared<Unary>(*u);

    copy->right = rSimp;
    return copy;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateBinary(Binary *b)
{
    std::shared_ptr<Expr> lSimp = b->left->Propagate(*this);
    std::shared_ptr<Expr> rSimp = b->right->Propagate(*this);

    std::shared_ptr<Binary> copy = std::make_shared<Binary>(*b);

    copy->left = lSimp;
    copy->right = rSimp;
    return copy;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateAssign(Assign *a)
{
    a->val = a->val->Propagate(*this);

    VarReference *vTarg = dynamic_cast<VarReference *>(a->target.get());
    std::shared_ptr<Literal> lVal = std::dynamic_pointer_cast<Literal>(a->val);

    if (vTarg != nullptr && lVal == nullptr)
        SetVariableValue(vTarg->GetType(), vTarg->name, nullptr);
    else if (vTarg != nullptr && lVal != nullptr)
        SetVariableValue(vTarg->GetType(), vTarg->name, lVal);

    ArrayIndex *aTarg = dynamic_cast<ArrayIndex *>(a->target.get());
    if (aTarg != nullptr)
    {
        aTarg->index = aTarg->index->Propagate(*this);

        Literal *lIndex = dynamic_cast<Literal *>(aTarg->index.get());
        VarReference *vName = dynamic_cast<VarReference *>(aTarg->name.get());

        if (vName == nullptr)
        {
            aTarg->name = aTarg->name->Propagate(*this);
            vName = dynamic_cast<VarReference *>(aTarg->name.get());
        }

        TypeData arrayType = aTarg->GetType();
        arrayType.isArray++;

        if (vName != nullptr && lIndex != nullptr && lVal != nullptr)
        {
            size_t index = std::stoi(lIndex->Loc().literal);
            SetArrayIndex(arrayType, vName->name, index, lVal);
        }
        else if (vName != nullptr && lIndex != nullptr && lVal == nullptr)
        {
            size_t index = std::stoi(lIndex->Loc().literal);
            SetArrayIndex(arrayType, vName->name, index, nullptr);
        }
    }

    return std::make_shared<Assign>(*a);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateVarReference(VarReference *vr)
{
    std::shared_ptr<Expr> varVal = GetVariableValue(vr->GetType(), vr->name);
    if (varVal == nullptr)
        return std::make_shared<VarReference>(*vr);

    didTreeChange = true;
    return varVal;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateFunctionCall(FunctionCall *fc)
{
    return std::make_shared<FunctionCall>(*fc);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateArrayIndex(ArrayIndex *ai)
{
    ai->index = ai->index->Propagate(*this);
    VarReference *vr = dynamic_cast<VarReference *>(ai->name.get());
    if (vr != nullptr)
    {
        TypeData arrayType = ai->GetType();
        arrayType.isArray++;
        std::shared_ptr<Expr> array = GetVariableValue(arrayType, vr->name);
        Literal *l = dynamic_cast<Literal *>(ai->index.get());

        if (array != nullptr && l != nullptr)
        {
            BracedInitialiser *bi = dynamic_cast<BracedInitialiser *>(array.get());
            didTreeChange = true;
            return bi->init[std::stoi(l->Loc().literal)];
        }
    }
    return std::make_shared<ArrayIndex>(*ai);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateBracedInitialiser(BracedInitialiser *ia)
{
    for (size_t i = 0; i < ia->init.size(); i++)
        ia->init[i] = ia->init[i]->Propagate(*this);

    return std::make_shared<BracedInitialiser>(*ia);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateDynamicAllocArray(DynamicAllocArray *da)
{
    da->size = da->size->Propagate(*this);
    return std::make_shared<DynamicAllocArray>(*da);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateFieldAccess(FieldAccess *fa)
{
    return std::make_shared<FieldAccess>(*fa);
}

std::shared_ptr<Expr> ConstantPropagator::PropagateTypeCast(TypeCast *gf)
{
    gf->arg = gf->arg->Propagate(*this);
    return std::make_shared<TypeCast>(*gf);
}

//------------------STATEMENTS---------------------//

void ConstantPropagator::PropagateExprStmt(ExprStmt *es)
{
    es->exp = es->exp->Propagate(*this);
}

void ConstantPropagator::PropagateDeclaredVar(DeclaredVar *dv)
{
    if (dv->value != nullptr)
    {
        std::shared_ptr<Expr> simp = dv->value->Propagate(*this);
        if (dynamic_cast<Literal *>(simp.get()) != nullptr)
            stack.push_back(LiteralValue(depth, dv->t, dv->name, simp));

        BracedInitialiser *bi = dynamic_cast<BracedInitialiser *>(simp.get());
        if (bi != nullptr)
        {
            bool isAllConstant = true;
            for (size_t i = 0; i < bi->init.size(); i++)
            {
                if (!IsConstant(bi->init[i]))
                {
                    isAllConstant = false;
                    break;
                }
            }

            if (isAllConstant)
                stack.push_back(LiteralValue(depth, dv->value->GetType(), dv->name, simp));
        }

        dv->value = simp;
    }
}

void ConstantPropagator::PropagateBlock(Block *b)
{
    depth++;

    for (auto &stmt : b->stmts)
        stmt->Propagate(*this);

    ClearCurrentDepth();
    depth--;
}

void ConstantPropagator::PropagateIfStmt(IfStmt *i)
{
    i->cond = i->cond->Propagate(*this);
    i->thenBranch->Propagate(*this);

    if (i->elseBranch != nullptr)
        i->elseBranch->Propagate(*this);
}

void ConstantPropagator::PropagateWhileStmt(WhileStmt *ws)
{
    ws->cond = ws->cond->Propagate(*this);
    ws->body->Propagate(*this);
}

void ConstantPropagator::PropagateFuncDecl(FuncDecl *fd)
{
    depth++;
    for (auto &stmt : fd->body)
        stmt->Propagate(*this);
    ClearCurrentDepth();
    depth--;
}

void ConstantPropagator::PropagateReturn(Return *r)
{
    r->retVal = r->retVal->Propagate(*this);
}

void ConstantPropagator::PropagateStructDecl(StructDecl *sd)
{
    depth++;
    for (auto &decl : sd->decls)
        decl->Propagate(*this);

    ClearCurrentDepth();
    depth--;
}

void ConstantPropagator::PropagateImportStmt(ImportStmt *)
{
}

void ConstantPropagator::PropagateThrow(Throw *t)
{
    t->exp = t->exp->Propagate(*this);
}

void ConstantPropagator::PropagateTryCatch(TryCatch *tc)
{
    tc->tryClause->Propagate(*this);
    tc->catchClause->Propagate(*this);
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> Literal::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateLiteral(this);
}

std::shared_ptr<Expr> Unary::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateUnary(this);
}

std::shared_ptr<Expr> Binary::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateBinary(this);
}

std::shared_ptr<Expr> Assign::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateAssign(this);
}

std::shared_ptr<Expr> VarReference::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateVarReference(this);
}

std::shared_ptr<Expr> FunctionCall::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateFunctionCall(this);
}

std::shared_ptr<Expr> ArrayIndex::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateArrayIndex(this);
}

std::shared_ptr<Expr> BracedInitialiser::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateBracedInitialiser(this);
}

std::shared_ptr<Expr> DynamicAllocArray::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateDynamicAllocArray(this);
}

std::shared_ptr<Expr> FieldAccess::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateFieldAccess(this);
}

std::shared_ptr<Expr> TypeCast::Propagate(ConstantPropagator &cp)
{
    return cp.PropagateTypeCast(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Propagate(ConstantPropagator &cp)
{
    cp.PropagateExprStmt(this);
}

void DeclaredVar::Propagate(ConstantPropagator &cp)
{
    cp.PropagateDeclaredVar(this);
}

void Block::Propagate(ConstantPropagator &cp)
{
    cp.PropagateBlock(this);
}

void IfStmt::Propagate(ConstantPropagator &cp)
{
    cp.PropagateIfStmt(this);
}

void WhileStmt::Propagate(ConstantPropagator &cp)
{
    cp.PropagateWhileStmt(this);
}

void FuncDecl::Propagate(ConstantPropagator &cp)
{
    cp.PropagateFuncDecl(this);
}

void Return::Propagate(ConstantPropagator &cp)
{
    cp.PropagateReturn(this);
}

void StructDecl::Propagate(ConstantPropagator &cp)
{
    cp.PropagateStructDecl(this);
}

void ImportStmt::Propagate(ConstantPropagator &cp)
{
    cp.PropagateImportStmt(this);
}

void Break::Propagate(ConstantPropagator &)
{
}

void Throw::Propagate(ConstantPropagator &cp)
{
    cp.PropagateThrow(this);
}

void TryCatch::Propagate(ConstantPropagator &cp)
{
    cp.PropagateTryCatch(this);
}