#pragma once
#include "common.h"

enum class Opcode : uint8_t
{
    POP,
    GET_C,

    // variables
    VAR_A,
    VAR_A_GLOBAL,

    VAR_D_GLOBAL,

    GET_V,
    GET_V_GLOBAL,

    // arrays
    ARR_D,
    ARR_INDEX,
    ARR_SET,
    ARR_ALLOC,

    STRING_INDEX,
    STRING_SET,

    // conditonals
    JUMP_IF_FALSE,
    JUMP,
    LOOP,

    // functions
    CALL_F,
    RETURN,
    NATIVE_CALL,

    // structs
    STRUCT_MEMBER,
    STRUCT_D,
    STRUCT_MEMBER_SET,

    // ADDITION
    I_ADD,
    DI_ADD,
    ID_ADD,
    D_ADD,
    // string concatenation
    S_ADD,

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

    // GT,
    I_GT,
    DI_GT,
    ID_GT,
    D_GT,

    // LT,
    I_LT,
    DI_LT,
    ID_LT,
    D_LT,

    // GEQ,
    I_GEQ,
    DI_GEQ,
    ID_GEQ,
    D_GEQ,

    // LEQ,
    I_LEQ,
    DI_LEQ,
    ID_LEQ,
    D_LEQ,

    // EQ_EQ,
    N_EQ_EQ,
    I_EQ_EQ,
    DI_EQ_EQ,
    ID_EQ_EQ,
    D_EQ_EQ,
    B_EQ_EQ,

    // BANG_EQ,
    N_BANG_EQ,
    I_BANG_EQ,
    DI_BANG_EQ,
    ID_BANG_EQ,
    D_BANG_EQ,
    B_BANG_EQ,

    BANG,

    NONE,
};

std::string ToString(Opcode o);

Opcode TokenToOpcode(TypeData l, TokenID t, TypeData r, bool isUnary);

struct Op
{
    Opcode code;
    uint8_t op;
};
