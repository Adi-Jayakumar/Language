#pragma once
#include "ASTPrinter.h"
#include "compileconst.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,
    VAR_D,
    GET_V,

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
    uint8_t operand;
};

struct Chunk
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    std::vector<VarID> vars;
    uint8_t depth = 0;
    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();
    size_t ResolveVariable(std::string &, uint8_t depth);
    void CleanUpVariables();
};

namespace NodeCompiler
{
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
} // namespace NodeCompiler