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

    CALL_F,
    NATIVE_FN,
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
    uint8_t op1;
};
