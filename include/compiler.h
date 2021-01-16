#pragma once
#include "ASTPrinter.h"
#include "maps.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,
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

struct CompileConst
{
    TypeID type;
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
    CompileConst(TypeID, std::string &);
};

struct CompileVar
{
    std::string name;
    Expr *value;
    CompileVar(std::string, Expr *);
};

std::ostream &operator<<(std::ostream &out, CompileConst &cc);

struct Chunk
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    std::vector<CompileVar> vars;
    Chunk() = default;
    ~Chunk() = default;

    void PrintCode();

    size_t ResolveVariable(std::string &);

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