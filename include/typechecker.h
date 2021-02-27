#pragma once
#include "common.h"
#include "perror.h"
#include "stmtnode.h"

struct VarID
{
    TypeData type;
    std::string name;
    uint8_t depth;
};

struct FuncID
{
    TypeData ret;
    std::string name;
    std::vector<TypeData> argtypes;
};

struct TypeChecker
{
    uint8_t depth;
    bool isInFunc = false;
    size_t funcVarBegin = 0;
    std::vector<VarID> vars;
    std::vector<FuncID> funcs{{{false, 0}, "Print", {{true, 0}}}, {{false, 4}, "ToString", {{false, 0}}}};

    bool hadError = false;

    TypeChecker() = default;
    void TypeCheck(std::shared_ptr<Stmt> &s);

    void TypeError(Token loc, std::string err);

    // resolve variables
    size_t ResolveVariable(std::string &name, Token loc);
    size_t CheckVariablesInFunction(std::string &name);
    bool IsVariableInScope(std::string &name);
    void CleanUpVariables();


    // returns true if that type can potentially be 'truthy'
    bool IsTruthy(const TypeData &cond);

    // returns true if the type valType can be assigned to a variable of type
    // var type
    bool CanAssign(const TypeData &varType, const TypeData &valType);

    // matches native functions' arguments
    bool MatchNativeArguments(std::vector<TypeData> &actual, std::vector<TypeData> &supplied);
    // resolve native functions
    size_t ResolveNativeFunctions(std::string &name, std::vector<TypeData> &argTypes);
    // resolve functions
    size_t ResolveFunction(std::string &name, std::vector<TypeData> &argtypes);

    // expression typechecking
    TypeData TypeOfLiteral(Literal *l);
    TypeData TypeOfUnary(Unary *u);
    TypeData TypeOfBinary(Binary *b);
    TypeData TypeOfAssign(Assign *a);
    TypeData TypeOfVarReference(VarReference *vr);
    TypeData TypeOfFunctionCall(FunctionCall *fc);
    TypeData TypeOfArrayIndex(ArrayIndex *ai);
    TypeData TypeOfInlineArray(InlineArray *ia);
    TypeData TypeOfDynamicAllocArray(DynamicAllocArray *da);

    // statment typechecking
    TypeData TypeOfExprStmt(ExprStmt *es);
    TypeData TypeOfDeclaredVar(DeclaredVar *v);
    TypeData TypeOfBlock(Block *b);
    TypeData TypeOfIfStmt(IfStmt *i);
    TypeData TypeOfWhileStmt(WhileStmt *ws);
    TypeData TypeOfFuncDecl(FuncDecl *fd);
    TypeData TypeOfReturn(Return *r);
};