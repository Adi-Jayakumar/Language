#pragma once
#include "nodesubstitution.h"
#include "perror.h"
#include "stmtnode.h"
#include "symboltable.h"
#include <cassert>

class PostConditionGenerator
{
    std::vector<std::vector<z3::expr>> post;
    std::vector<z3::expr> conditions;
    std::vector<SP<Stmt>> program;
    SymbolTable symbols; // for function calls
    FuncDecl *cur_func;
    z3::context context;

public:
    // Should pass in the SymbolTable used to StaticAnalyse the code
    PostConditionGenerator(const SymbolTable &_symbols) : symbols(_symbols){
                                                              // post.push_back(std::vector<z3::expr>());
                                                          };

    void Generate(FuncDecl *fd);
    z3::expr GetZ3Post();

    void PostConditionError(Token loc, std::string err);
    void ReplaceFunctionCall(SP<Expr> &post);

    z3::expr MAKE_RETURN(const TypeData &type, const z3::expr &val, const Token &loc);
    void AddReturnValue(const z3::expr &ret);
    void AddCondition(const z3::expr &c);
    void RemoveLastCondition();

    // expression generation
    z3::expr GenerateFromLiteral(Literal *l);
    z3::expr GenerateFromUnary(Unary *u);
    z3::expr GenerateFromBinary(Binary *b);
    z3::expr GenerateFromAssign(Assign *a);
    z3::expr GenerateFromVarReference(VarReference *vr);
    z3::expr GenerateFromFunctionCall(FunctionCall *fc);
    z3::expr GenerateFromArrayIndex(ArrayIndex *ai);
    z3::expr GenerateFromBracedInitialiser(BracedInitialiser *ia);
    z3::expr GenerateFromDynamicAllocArray(DynamicAllocArray *da);
    z3::expr GenerateFromFieldAccess(FieldAccess *fa);
    z3::expr GenerateFromTypeCast(TypeCast *tc);
    z3::expr GenerateFromSequence(Sequence *s);

    // statement generation
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