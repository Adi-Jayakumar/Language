#pragma once
#include "stmtnode.h"

namespace ASTPrinter
{
    // expression printing
    void PrintLiteral(Literal *l, std::ostream &out);
    void PrintUnary(Unary *u, std::ostream &out);
    void PrintBinary(Binary *b, std::ostream &out);
    void PrintAssign(Assign *a, std::ostream &out);
    void PrintVarReference(VarReference *vr, std::ostream &out);
    void PrintFunctionCall(FunctionCall *fc, std::ostream &out);
    void PrintArrayIndex(ArrayIndex *ai, std::ostream &out);
    void PrintBracedInitialiser(BracedInitialiser *ia, std::ostream &out);
    void PrintDynamicAllocArray(DynamicAllocArray *da, std::ostream &out);
    void PrintFieldAccess(FieldAccess *fa, std::ostream &out);
    void PrintTypeCast(TypeCast *gf, std::ostream &out);

    // statment printing
    void PrintExprStmt(ExprStmt *es, std::ostream &out);
    void PrintDeclaredVar(DeclaredVar *v, std::ostream &out);
    void PrintBlock(Block *b, std::ostream &out);
    void PrintIfStmt(IfStmt *i, std::ostream &out);
    void PrintWhileStmt(WhileStmt *ws, std::ostream &out);
    void PrintFuncDecl(FuncDecl *fd, std::ostream &out);
    void PrintReturn(Return *r, std::ostream &out);
    void PrintStructDecl(StructDecl *sd, std::ostream &out);
    void PrintImportStmt(ImportStmt *is, std::ostream &out);
}; // namespace ASTPrinter