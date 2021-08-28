#pragma once
#include "stmtnode.h"
#include "symboltable.h"

class PostCondition
{
    SymbolTable Symbols;
    std::vector<std::vector<SP<Expr>>> post;

public:
    PostCondition() = default;

    void PostConditionError(Token loc, std::string err);
    void Analyse(std::vector<SP<Stmt>> &program);

    // expression analysis
    void GenerateFromLiteral(Literal *l);
    void GenerateFromUnary(Unary *u);
    void GenerateFromBinary(Binary *b);
    void GenerateFromAssign(Assign *a);
    void GenerateFromVarReference(VarReference *vr);
    void GenerateFromFunctionCall(FunctionCall *fc);
    void GenerateFromArrayIndex(ArrayIndex *ai);
    void GenerateFromBracedInitialiser(BracedInitialiser *ia);
    void GenerateFromDynamicAllocArray(DynamicAllocArray *da);
    void GenerateFromFieldAccess(FieldAccess *fa);
    void GenerateFromTypeCast(TypeCast *tc);
    void GenerateFromSequence(Sequence *s);

    // statement analysis
    void GenerateFromExprStmt(ExprStmt *es);
    void GenerateFromDeclaredVar(DeclaredVar *dv);
    void GenerateFromBlock(Block *b);
    void GenerateFromIfStmt(IfStmt *i);
    void GenerateFromWhileStmt(WhileStmt *ws);
    void GenerateFromFuncDecl(FuncDecl *fd);
    void GenerateFromReturn(Return *r);
    void GenerateFromStructDecl(StructDecl *sd);
    void GenerateFromImportStmt(ImportStmt *is);
    void GenerateFromBreak(Break *b);
    void GenerateFromThrow(Throw *t);
    void GenerateFromTryCatch(TryCatch *tc);
};