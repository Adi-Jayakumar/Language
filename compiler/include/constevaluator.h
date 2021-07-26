#pragma once
#include "literalevaluator.h"

bool IsLiteral(std::shared_ptr<Expr> exp);

namespace ConstantEvaluator
{
    // expression evaluator
    std::shared_ptr<Expr> EvaluateLiteral(Literal *l);
    std::shared_ptr<Expr> EvaluateUnary(Unary *u);
    std::shared_ptr<Expr> EvaluateBinary(Binary *b);
    std::shared_ptr<Expr> EvaluateAssign(Assign *a);
    std::shared_ptr<Expr> EvaluateVarReference(VarReference *vr);
    std::shared_ptr<Expr> EvaluateFunctionCall(FunctionCall *fc);
    std::shared_ptr<Expr> EvaluateArrayIndex(ArrayIndex *ai);
    std::shared_ptr<Expr> EvaluateBracedInitialiser(BracedInitialiser *ia);
    std::shared_ptr<Expr> EvaluateDynamicAllocArray(DynamicAllocArray *da);
    std::shared_ptr<Expr> EvaluateFieldAccess(FieldAccess *fa);
    std::shared_ptr<Expr> EvaluateTypeCast(TypeCast *gf);

    // statment evaluator
    void EvaluateExprStmt(ExprStmt *es);
    void EvaluateDeclaredVar(DeclaredVar *v);
    void EvaluateBlock(Block *b);
    void EvaluateIfStmt(IfStmt *i);
    void EvaluateWhileStmt(WhileStmt *ws);
    void EvaluateFuncDecl(FuncDecl *fd);
    void EvaluateReturn(Return *r);
    void EvaluateStructDecl(StructDecl *sd);
    void EvaluateImportStmt(ImportStmt *is);
    void EvaluateThrow(Throw *t);
    void EvaluateTryCatch(TryCatch *tc);
} // namespace ConstantEvaluator
