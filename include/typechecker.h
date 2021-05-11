#pragma once
#include "common.h"
#include "idstructs.h"
#include "perror.h"
#include "stmtnode.h"
#include "symboltable.h"

bool IsTruthy(const TypeData &);

struct TypeChecker
{
    bool CanAssign(const TypeData &, const TypeData &);
    bool MatchInitialiserToStruct(const std::vector<TypeData> &, const std::vector<TypeData> &);

    void TypeError(Token loc, std::string err);
    SymbolTable Symbols;

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
    TypeData TypeOfExprStmt(ExprStmt *es);
    TypeData TypeOfDeclaredVar(DeclaredVar *v);
    TypeData TypeOfBlock(Block *b);
    TypeData TypeOfIfStmt(IfStmt *i);
    TypeData TypeOfWhileStmt(WhileStmt *ws);
    TypeData TypeOfFuncDecl(FuncDecl *fd);
    TypeData TypeOfReturn(Return *r);
    TypeData TypeOfStructDecl(StructDecl *sd);
    TypeData TypeOfImportStmt(ImportStmt *is);
};