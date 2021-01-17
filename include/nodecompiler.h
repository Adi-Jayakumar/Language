#pragma once
#include "ASTPrinter.h"
#include "compileconst.h"
#include "perror.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,
    VAR_D,
    VAR_A,
    GET_V,
    JUMP_IF_FALSE,
    JUMP,

    ADD,
    SUB,
    MUL,
    DIV,

    GT,
    LT,
    GEQ,
    LEQ,

    EQ_EQ,
    BANG_EQ,
    NONE,
};

std::string ToString(Opcode o);

Opcode TokenToOpcode(TokenID t);

struct Op
{
    Opcode code;
    uint16_t operand;
};

struct Chunk
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    std::vector<VarID> vars;
    uint16_t depth = 0;
    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
    size_t ResolveVariable(std::string &, uint16_t depth);
    void CleanUpVariables();
};

namespace NodeCompiler
{
    void CompileError(std::string err);

    // expression compiling
    void CompileLiteral(Literal *l, Chunk &c);
    void CompileUnary(Unary *u, Chunk &c);
    void CompileBinary(Binary *b, Chunk &c);
    void CompileAssign(Assign *a, Chunk &c);
    void CompileVarReference(VarReference *vr, Chunk &c);

    // statement compiling
    void CompileExprStmt(ExprStmt *es, Chunk &c);
    void CompileDeclaredVar(DeclaredVar *dv, Chunk &c);
    void CompileBlock(Block *b, Chunk &c);
    void CompileIfStmt(IfStmt*i, Chunk &c);
} // namespace NodeCompiler