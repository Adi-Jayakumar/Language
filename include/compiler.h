#pragma once
#include "stmtnode.h"
#include "ASTPrinter.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,

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

struct CompileConst
{
    TypeID type;
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
    CompileConst(TypeID, std::string&);
};

std::ostream &operator<<(std::ostream &out, CompileConst &cc);


struct Chunk
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    Chunk() = default;
    ~Chunk();

    void PrintCode();

    // expression compiling
    void CompileLiteral(Literal *l);
    void CompileUnary(Unary *u);
    void CompileBinary(Binary *b);
    void CompileAssign(Assign *a);
    void CompileVarReference(VarReference *vr);

    // statement compiling
    void CompileExprStmt(ExprStmt *es);
    void CompileDeclaredVar(DeclaredVar *dv);
    void CompileBlock(Block *b);
};