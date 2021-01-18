#pragma once
#include "perror.h"
#include "stmtnode.h"
#include "common.h"

struct TypeChecker
{
    uint16_t depth;
    std::vector<VarID> vars;

    TypeChecker() = default;
    void TypeCheck(std::shared_ptr<Stmt> &s);

    void TypeError(Token loc, std::string err);

    TypeID ResolveVariable(std::string &name);
    TypeID ResolveVariableInScope(std::string &name);
    // void CleanUpVariables();

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
    TypeID TypeOfIfStmt(IfStmt* i);
};