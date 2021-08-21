#pragma once
#include "common.h"
#include "idstructs.h"
#include "internaltypes.h"
#include "op.h"
#include "perror.h"
#include "stmtnode.h"
#include "symboltable.h"
#include <cassert>
#include <stack>

bool IsTruthy(const TypeData &);

struct StaticAnalyser
{
    SymbolTable Symbols;
    FuncDecl *curFunc = nullptr;
    size_t index;
    std::vector<std::shared_ptr<Stmt>> prog;

    TypeSubstitution *curTS;
    std::stack<TypeSubstitution> typestack;

    void TypeError(Token loc, std::string err);
    bool MatchInitialiserToStruct(const std::vector<TypeData> &, const std::vector<TypeData> &);

    StaticAnalyser()
    {
        typestack.push(TypeSubstitution());
        curTS = &typestack.top();
    };
    void operator()(std::vector<std::shared_ptr<Stmt>> &_prog);

    void TypeCheck(std::shared_ptr<Stmt> &s);

    // expression typechecking
    TypeData TypeOfLiteral(Literal *l);
    TypeData TypeOfUnary(Unary *u);
    TypeData TypeOfBinary(Binary *b);
    TypeData TypeOfAssign(Assign *a);
    TypeData TypeOfVarReference(VarReference *vr);
    TypeData TypeOfFunctionCall(FunctionCall *fc);
    TypeData TypeOfArrayIndex(ArrayIndex *ai);
    TypeData TypeOfBracedInitialiser(BracedInitialiser *ia);
    TypeData TypeOfDynamicAllocArray(DynamicAllocArray *da);
    TypeData TypeOfFieldAccess(FieldAccess *fa);
    TypeData TypeOfTypeCast(TypeCast *gf);

    // statment typechecking
    void TypeOfExprStmt(ExprStmt *es);
    void TypeOfDeclaredVar(DeclaredVar *v);
    void TypeOfBlock(Block *b);
    void TypeOfIfStmt(IfStmt *i);
    void TypeOfWhileStmt(WhileStmt *ws);
    void TypeOfFuncDecl(FuncDecl *fd);
    void TypeOfReturn(Return *r);
    void TypeOfStructDecl(StructDecl *sd);
    void TypeOfImportStmt(ImportStmt *is);
    void TypeOfThrow(Throw *t);
    void TypeOfTryCatch(TryCatch *tc);
};