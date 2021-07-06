#pragma once
#include <string>

enum class Opcode : uint8_t
{
    POP,
    LOAD_INT,
    LOAD_DOUBLE,
    LOAD_BOOL,
    LOAD_STRING,
    LOAD_CHAR,

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
    STRUCT_ALLOC,

    STRING_INDEX,
    STRING_SET,

    // conditonals
    JUMP_IF_FALSE,
    JUMP,
    LOOP,

    // functions
    CALL_F,
    RETURN,
    RETURN_VOID,
    NATIVE_CALL,
    PRINT,

    // structs
    STRUCT_MEMBER,
    STRUCT_D,
    STRUCT_MEMBER_SET,
    CAST,

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

struct Op
{
    Opcode code;
    uint8_t op;
};
