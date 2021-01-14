#pragma once
#include "stmtnode.h"

enum class Opcode
{
    INT_C,
    DOUBLE_C,
    BOOL_C,

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
};

struct Op
{
    Opcode code;
    uint8_t operand;
};

struct Chunk
{
    std::vector<Op> result;
    std::vector<Literal*> constants;
    ~Chunk();
};

namespace NodeCompiler
{
    // expressio node compiling
    void CompileLiteral(Literal* l, std::vector<Op> &out);
    void CompileUnary(Unary* u, std::vector<Op> &out);
    void CompileBinary(Binary* b, std::vector<Op> &out);
    void CompileAssign(Assign* a, std::vector<Op> &out);
    void CompileVarReference(VarReference* vr, std::vector<Op> &out);

    // statement compiling
    void CompileExprStmt(ExprStmt* es, std::vector<Op> &out);
    void CompileDeclaredVar(DeclaredVar* dv, std::vector<Op> &out);
    void CompileBlock(Block* b, std::vector<Op> &out);
}