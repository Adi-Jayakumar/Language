#include "constevaluator.h"

//------------------EXPRESSION--------------------//

std::shared_ptr<Expr> ConstantEvaluator::EvaluateLiteral(Literal *, bool &)
{
    return nullptr;
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

void Break::Evaluate(bool &)
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