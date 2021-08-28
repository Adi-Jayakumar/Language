#pragma once
#include "stmtnode.h"
#include "symboltable.h"

class PostCondition
{
    SymbolTable Symbols;
    std::vector<std::vector<SP<Expr>>> post;
    std::vector<SP<Expr>> conditions;

public:
    PostCondition()
    {
        post.push_back(std::vector<SP<Expr>>());
    };

    void PostConditionError(Token loc, std::string err);
    void Analyse(std::vector<SP<Stmt>> &program);

    void AddReturnValue(SP<Expr> &ret);
    void AddCondition(const SP<Expr> &c);
    void RemoveLastCondition();

    void PostConditionError(Token loc, std::string err);

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