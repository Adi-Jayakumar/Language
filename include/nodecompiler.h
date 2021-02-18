#pragma once
#include "ASTPrinter.h"
#include "compiler.h"

namespace NodeCompiler
{

    static const std::unordered_map<std::string, TypeID> NativeReturn{
        {"Print", 0},
    };


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
    void CompileArrayDecl(ArrayDecl *ad, Compiler &c);
    void CompileBlock(Block *b, Compiler &c);
    void CompileIfStmt(IfStmt *i, Compiler &c);
    void CompileWhileStmt(WhileStmt *ws, Compiler &c);
    void CompileFuncDecl(FuncDecl *fd, Compiler &c);
    void CompileReturn(Return *r, Compiler &c);
} // namespace NodeCompiler