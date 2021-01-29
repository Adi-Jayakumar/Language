#pragma once
#include "common.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,
    VAR_A,
    GET_V,

    JUMP_IF_FALSE,
    JUMP,
    LOOP,

    CALL_F,
    RETURN,

    // ADDITION
    I_ADD,
    DI_ADD,
    ID_ADD,
    D_ADD,

    // SUBTRACTION
    I_SUB,
    DI_SUB,
    ID_SUB,
    D_SUB,

    // MULTIPLICATION
    I_MUL,
    DI_MUL,
    ID_MUL,
    D_MUL,

    // DIVISION
    I_DIV,
    DI_DIV,
    ID_DIV,
    D_DIV,

    GT,
    LT,
    GEQ,
    LEQ,

    EQ_EQ,
    BANG_EQ,
    NONE,
};

std::string ToString(Opcode o);

Opcode TokenToOpcode(TypeID l, TokenID t, TypeID r);

struct Op
{
    Opcode code;
    uint8_t op1;
};
