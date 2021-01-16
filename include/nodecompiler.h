#pragma once
#include "ASTPrinter.h"
#include "maps.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,
    GET_V,
    VAR_D,

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
    CompileConst() = default;
    CompileConst(TypeID, std::string &);
};

struct CompileVar
{
    std::string name;
    CompileConst decl;
    CompileVar(std::string);
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