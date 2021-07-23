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
            std::cout << "REPLACING " << (val == nullptr ? "NULLPTR" : "SMTH ELSE") << " at index " << i << std::endl;
            vv->value = val;
            break;
        }
    }
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