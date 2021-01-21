#pragma once
#include "ASTPrinter.h"
#include "perror.h"
#include "compiler.h"

namespace NodeCompiler
{
    void CompileError(std::string err);

    // expression compiling
    void CompileLiteral(Literal *l, Compiler &c);
    void CompileUnary(Unary *u, Compiler &c);
    void CompileBinary(Binary *b, Compiler &c);
    void CompileAssign(Assign *a, Compiler &c);
    void CompileVarReference(VarReference *vr, Compiler &c);
    void CompileFunctionCall(FunctionCall *fc, Compiler &c);

    // statement compiling
    void CompileExprStmt(ExprStmt *es, Compiler &c);
    void CompileDeclaredVar(DeclaredVar *dv, Compiler &c);
    void CompileBlock(Block *b, Compiler &c);
    void CompileIfStmt(IfStmt *i, Compiler &c);
    void CompileFuncDecl(FuncDecl *fd, Compiler &c);
    void CompileReturn(Return *r, Compiler &c);
} // namespace NodeCompiler