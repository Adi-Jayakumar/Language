#pragma once
#include "common.h"
#include "perror.h"
#include "stmtnode.h"

struct VarID
{
    TypeID type;
    std::string name;
    uint8_t depth;
};

struct FuncID
{
    TypeID ret;
    std::string name;
    std::vector<TypeID> argtypes;
};

struct TypeChecker
{
    uint8_t depth;
    bool isInFunc = false;
    size_t funcVarBegin = 0;
    std::vector<VarID> vars;
    std::vector<FuncID> funcs;

    TypeChecker() = default;
    void TypeCheck(std::shared_ptr<Stmt> &s);

    void TypeError(Token loc, std::string err);

    // resolve variables
    TypeID ResolveVariable(std::string &name);
    TypeID CheckVariablesInFunction(std::string &name);
    bool IsVariableInScope(std::string &name);
    void CleanUpVariables();

    // resolve functions
    TypeID ResolveFunction(std::string &name, std::vector<TypeID> &argtypes);

    // expression typechecking
    TypeID TypeOfLiteral(Literal *l);
    TypeID TypeOfUnary(Unary *u);
    TypeID TypeOfBinary(Binary *b);
    TypeID TypeOfAssign(Assign *a);
    TypeID TypeOfVarReference(VarReference *vr);
    TypeID TypeOfFunctionCall(FunctionCall *fc);

    // statment typechecking
    TypeID TypeOfExprStmt(ExprStmt *es);
    TypeID TypeOfDeclaredVar(DeclaredVar *v);
    TypeID TypeOfArrayDecl(ArrayDecl *ad);
    TypeID TypeOfBlock(Block *b);
    TypeID TypeOfIfStmt(IfStmt *i);
    TypeID TypeOfWhileStmt(WhileStmt *ws);
    TypeID TypeOfFuncDecl(FuncDecl *fd);
    TypeID TypeOfReturn(Return *r);
};