#pragma once
#include "literalevaluator.h"

namespace ConstantEvaluator
{
    // expression evaluator
    std::shared_ptr<Expr> EvaluateLiteral(Literal *l, bool &didSimp);
    std::shared_ptr<Expr> EvaluateUnary(Unary *u, bool &didSimp);
    std::shared_ptr<Expr> EvaluateBinary(Binary *b, bool &didSimp);
    std::shared_ptr<Expr> EvaluateAssign(Assign *a, bool &didSimp);
    std::shared_ptr<Expr> EvaluateVarReference(VarReference *vr, bool &didSimp);
    std::shared_ptr<Expr> EvaluateFunctionCall(FunctionCall *fc, bool &didSimp);
    std::shared_ptr<Expr> EvaluateArrayIndex(ArrayIndex *ai, bool &didSimp);
    std::shared_ptr<Expr> EvaluateBracedInitialiser(BracedInitialiser *ia, bool &didSimp);
    std::shared_ptr<Expr> EvaluateDynamicAllocArray(DynamicAllocArray *da, bool &didSimp);
    std::shared_ptr<Expr> EvaluateFieldAccess(FieldAccess *fa, bool &didSimp);
    std::shared_ptr<Expr> EvaluateTypeCast(TypeCast *gf, bool &didSimp);

    // statment evaluator
    void EvaluateExprStmt(ExprStmt *es, bool &didSimp);
    void EvaluateDeclaredVar(DeclaredVar *v, bool &didSimp);
    void EvaluateBlock(Block *b, bool &didSimp);
    void EvaluateIfStmt(IfStmt *i, bool &didSimp);
    void EvaluateWhileStmt(WhileStmt *ws, bool &didSimp);
    void EvaluateFuncDecl(FuncDecl *fd, bool &didSimp);
    void EvaluateReturn(Return *r, bool &didSimp);
    void EvaluateStructDecl(StructDecl *sd, bool &didSimp);
    void EvaluateImportStmt(ImportStmt *is, bool &didSimp);
    void EvaluateThrow(Throw *t, bool &didSimp);
    void EvaluateTryCatch(TryCatch *tc, bool &didSimp);
} // namespace ConstantEvaluator
