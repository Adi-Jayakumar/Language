#pragma once
#include "ASTPrinter.h"
#include "perror.h"
#include "chunk.h"

namespace NodeCompiler
{
    void CompileError(std::string err);

    // expression compiling
    void CompileLiteral(Literal *l, Chunk &c);
    void CompileUnary(Unary *u, Chunk &c);
    void CompileBinary(Binary *b, Chunk &c);
    void CompileAssign(Assign *a, Chunk &c);
    void CompileVarReference(VarReference *vr, Chunk &c);
    void CompileFunctionCall(FunctionCall *fc, Chunk &c);

    // statement compiling
    void CompileExprStmt(ExprStmt *es, Chunk &c);
    void CompileDeclaredVar(DeclaredVar *dv, Chunk &c);
    void CompileBlock(Block *b, Chunk &c);
    void CompileIfStmt(IfStmt *i, Chunk &c);
    void CompileFuncDecl(FuncDecl *fd, Chunk &c);
    void CompileReturn(Return *r, Chunk &c);
} // namespace NodeCompiler