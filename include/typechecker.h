#pragma once
#include "common.h"
#include "perror.h"
#include "stmtnode.h"

struct TypeChecker
{
    uint16_t depth;
    bool isInFunc = false;
    size_t funcVarBegin = 0;
    std::vector<VarID> vars;

    TypeChecker() = default;
    void TypeCheck(std::shared_ptr<Stmt> &s);

    void TypeError(Token loc, std::string err);

    TypeID ResolveVariable(std::string &name);
    TypeID CheckVariablesInFunction(std::string &name);
    bool IsVariableInScope(std::string &name);
    void CleanUpVariables();

    // expression typechecking
    TypeID TypeOfLiteral(Literal *l);
    TypeID TypeOfUnary(Unary *u);
    TypeID TypeOfBinary(Binary *b);
    TypeID TypeOfAssign(Assign *a);
    TypeID TypeOfVarReference(VarReference *vr);

    // statment typechecking
    TypeID TypeOfExprStmt(ExprStmt *es);
    TypeID TypeOfDeclaredVar(DeclaredVar *v);
    TypeID TypeOfBlock(Block *b);
    TypeID TypeOfIfStmt(IfStmt *i);
    TypeID TypeOfFuncDecl(FuncDecl *fd);
    TypeID TypeOfReturn(Return *r);
};