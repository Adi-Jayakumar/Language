#pragma once
#include "common.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,
    VAR_D,
    VAR_A,
    GET_V,
    DEL_V,

    JUMP_IF_FALSE,
    JUMP,

    CALL_F,
    RETURN,

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
    uint16_t op1;
    uint16_t op2;
};
