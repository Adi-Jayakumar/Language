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
    for (int i = stack.size() - 1; i >= 0; i--)
    {
        VariableValue *vv = &stack[i];
        if (type == vv->type && name == vv->name)
        {
            vv->value = val;
            break;
        }
    }
}

std::shared_ptr<Expr> ConstantPropagator::PropagateUnary(Unary *u)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateBinary(Binary *b)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateAssign(Assign *a)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateVarReference(VarReference *vr)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateFunctionCall(FunctionCall *fc)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateArrayIndex(ArrayIndex *ai)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateBracedInitialiser(BracedInitialiser *ia)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateDynamicAllocArray(DynamicAllocArray *da)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateFieldAccess(FieldAccess *fa)
{
    return nullptr;
}

std::shared_ptr<Expr> ConstantPropagator::PropagateTypeCast(TypeCast *gf)
{
    return nullptr;
}

// statment interface
void ConstantPropagator::PropagateExprStmt(ExprStmt *es)
{
}
void ConstantPropagator::PropagateDeclaredVar(DeclaredVar *v)
{
}
void ConstantPropagator::PropagateBlock(Block *b)
{
}
void ConstantPropagator::PropagateIfStmt(IfStmt *i)
{
}
void ConstantPropagator::PropagateWhileStmt(WhileStmt *ws)
{
}
void ConstantPropagator::PropagateFuncDecl(FuncDecl *fd)
{
}
void ConstantPropagator::PropagateReturn(Return *r)
{
}
void ConstantPropagator::PropagateStructDecl(StructDecl *sd)
{
}
void ConstantPropagator::PropagateImportStmt(ImportStmt *is)
{
}
void ConstantPropagator::PropagateThrow(Throw *t)
{
}
void ConstantPropagator::PropagateTryCatch(TryCatch *tc)
{
}

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> Literal::Propagate(ConstantPropagator &)
{
    return nullptr;
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