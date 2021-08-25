#pragma once
#include "ASTPrinter.h"
#include "compiler.h"
#include "internaltypes.h"
#include <cassert>

Opcode TokenToOpcode(TypeData l, TokenID t, TypeData r, bool isUnary);

namespace NodeCompiler
{

    // expression compiling
    TypeData CompileLiteral(Literal *l, Compiler &c);
    TypeData CompileUnary(Unary *u, Compiler &c);
    TypeData CompileBinary(Binary *b, Compiler &c);
    TypeData CompileAssign(Assign *a, Compiler &c);
    TypeData CompileVarReference(VarReference *vr, Compiler &c);
    TypeData CompileFunctionCall(FunctionCall *fc, Compiler &c);
    TypeData CompileArrayIndex(ArrayIndex *ai, Compiler &c);
    TypeData CompileBracedInitialiser(BracedInitialiser *ia, Compiler &c);
    TypeData CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c);
    TypeData CompileFieldAccess(FieldAccess *fa, Compiler &c);
    TypeData CompileTypeCast(TypeCast *tc, Compiler &c);

    // statement compiling
    void CompileExprStmt(ExprStmt *es, Compiler &c);
    void CompileDeclaredVar(DeclaredVar *dv, Compiler &c);
    void CompileBlock(Block *b, Compiler &c);
    void CompileIfStmt(IfStmt *i, Compiler &c);
    void CompileWhileStmt(WhileStmt *ws, Compiler &c);
    void CompileFuncDecl(FuncDecl *fd, Compiler &c);
    void CompileReturn(Return *r, Compiler &c);
    void CompileStructDecl(StructDecl *sd, Compiler &c);
    void CompileImportStmt(ImportStmt *is, Compiler &C);
    void CompileBreak(Break *b, Compiler &c);
    void CompileThrow(Throw *t, Compiler &c);
    void CompilerTryCatch(TryCatch *tc, Compiler &c);
} // namespace NodeCompiler