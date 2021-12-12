#pragma once
#include "nodesubstitution.h"
#include "perror.h"
#include "stmtnode.h"
#include "symboltable.h"
#include <cassert>

class PostConditionGenerator
{
    std::vector<std::vector<SP<Expr>>> post;
    std::vector<SP<Expr>> conditions;
    std::vector<SP<Stmt>> program;
    SymbolTable symbols; // for function calls

public:
    // Should pass in the SymbolTable used to StaticAnalyse the code
    PostConditionGenerator(const SymbolTable &_symbols) : symbols(_symbols)
    {
        post.push_back(std::vector<SP<Expr>>());
    };

    void PostConditionError(Token loc, std::string err);
    std::vector<std::vector<SP<Expr>>> Generate(SP<FuncDecl> &function, std::vector<SP<Stmt>> &_program);

    void ReplaceFunctionCallInPost(std::vector<std::vector<SP<Expr>>> &post);
    // Replaces a function call with its postcondition but function arguments
    // substituted for the function all's arguments
    void ReplaceFunctionCall(SP<Expr> &post);

    void AddReturnValue(const SP<Expr> &ret);
    void AddCondition(const SP<Expr> &c);
    void RemoveLastCondition();

    // statement analysis
    void GenerateFromExprStmt(ExprStmt *es);
    void GenerateFromDeclaredVar(DeclaredVar *dv);
    void GenerateFromBlock(Block *b);
    void GenerateFromIfStmt(IfStmt *i);
    void GenerateFromWhileStmt(WhileStmt *ws);
    void GenerateFromFuncDecl(FuncDecl *fd);
    void GenerateFromTemplateDecl(TemplateDecl *td);
    void GenerateFromReturn(Return *r);
    void GenerateFromStructDecl(StructDecl *sd);
    void GenerateFromImportStmt(ImportStmt *is);
    void GenerateFromBreak(Break *b);
    void GenerateFromThrow(Throw *t);
    void GenerateFromTryCatch(TryCatch *tc);
};