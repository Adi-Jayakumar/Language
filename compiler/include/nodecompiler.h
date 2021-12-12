#pragma once
#include "ASTPrinter.h"
#include "compiler.h"
#include "internaltypes.h"
#include <cassert>

Opcode TokenToOpcode(TypeData l, TokenID t, TypeData r, bool isUnary);

namespace NodeCompiler
{
    // expression compiling
    void CompileLiteral(Literal *l, Compiler &c);
    void CompileUnary(Unary *u, Compiler &c);
    void CompileBinary(Binary *b, Compiler &c);
    void CompileAssign(Assign *a, Compiler &c);
    void CompileVarReference(VarReference *vr, Compiler &c);
    void CompileFunctionCall(FunctionCall *fc, Compiler &c);
    void CompileArrayIndex(ArrayIndex *ai, Compiler &c);
    void CompileBracedInitialiser(BracedInitialiser *ia, Compiler &c);
    void CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c);
    void CompileFieldAccess(FieldAccess *fa, Compiler &c);
    void CompileTypeCast(TypeCast *tc, Compiler &c);
    void CompileSequence(Sequence *s, Compiler &c);

    // statement compiling
    void CompileExprStmt(ExprStmt *es, Compiler &c);
    void CompileDeclaredVar(DeclaredVar *dv, Compiler &c);
    void CompileBlock(Block *b, Compiler &c);
    void CompileIfStmt(IfStmt *i, Compiler &c);
    void CompileWhileStmt(WhileStmt *ws, Compiler &c);
    void CompileFuncDecl(FuncDecl *fd, Compiler &c);
    void CompileTemplateDecl(TemplateDecl *td, Compiler &c);
    void CompileReturn(Return *r, Compiler &c);
    void CompileStructDecl(StructDecl *sd, Compiler &c);
    void CompileImportStmt(ImportStmt *is, Compiler &C);
    void CompileBreak(Break *b, Compiler &c);
    void CompileThrow(Throw *t, Compiler &c);
    void CompileTryCatch(TryCatch *tc, Compiler &c);
} // namespace NodeCompiler