#pragma once
#include "stmtnode.h"
#include <memory>

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
    std::shared_ptr<Stmt> EvaluateExprStmt(ExprStmt *es);
    std::shared_ptr<Stmt> EvaluateDeclaredVar(DeclaredVar *v);
    std::shared_ptr<Stmt> EvaluateBlock(Block *b);
    std::shared_ptr<Stmt> EvaluateIfStmt(IfStmt *i);
    std::shared_ptr<Stmt> EvaluateWhileStmt(WhileStmt *ws);
    std::shared_ptr<Stmt> EvaluateFuncDecl(FuncDecl *fd);
    std::shared_ptr<Stmt> EvaluateReturn(Return *r);
    std::shared_ptr<Stmt> EvaluateStructDecl(StructDecl *sd);
    std::shared_ptr<Stmt> EvaluateImportStmt(ImportStmt *is);
    std::shared_ptr<Stmt> EvaluateThrow(Throw *t);
    std::shared_ptr<Stmt> EvaluateTryCatch(TryCatch *tc);
} // namespace ConstantEvaluator
