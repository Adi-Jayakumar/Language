#include "op.h"

std::string ToString(Opcode o)
{
    switch (o)
    {
    case Opcode::POP:
    {
        return "POP";
    }
    case Opcode::GET_C:
    {
        return "GET_C";
    }
    case Opcode::GET_V:
    {
        return "GET_V";
    }
    case Opcode::VAR_A:
    {
        return "VAR_A";
    }
    case Opcode::JUMP_IF_FALSE:
    {
        return "JUMP_IF_FALSE";
    }
    case Opcode::JUMP:
    {
        return "JUMP";
    }
    case Opcode::LOOP:
    {
        return "LOOP";
    }
    case Opcode::CALL_F:
    {
        return "CALL_F";
    }
    case Opcode::RETURN:
    {
        return "RETURN";
    }
    case Opcode::I_ADD:
    {
        return "I_ADD";
    }
    case Opcode::DI_ADD:
    {
        return "DI_ADD";
    }
    case Opcode::ID_ADD:
    {
        return "ID_ADD";
    }
    case Opcode::D_ADD:
    {
        return "D_ADD";
    }
    case Opcode::I_SUB:
    {
        return "I_SUB";
    }
    case Opcode::DI_SUB:
    {
        return "DI_SUB";
    }
    case Opcode::ID_SUB:
    {
        return "ID_SUB";
    }
    case Opcode::D_SUB:
    {
        return "D_SUB";
    }
    case Opcode::I_MUL:
    {
        return "I_MUL";
    }
    case Opcode::DI_MUL:
    {
        return "DI_MUL";
    }
    case Opcode::ID_MUL:
    {
        return "ID_MUL";
    }
    case Opcode::D_MUL:
    {
        return "D_MUL";
    }
    case Opcode::I_DIV:
    {
        return "I_DIV";
    }
    case Opcode::DI_DIV:
    {
        return "DI_DIV";
    }
    case Opcode::ID_DIV:
    {
        return "ID_DIV";
    }
    case Opcode::D_DIV:
    {
        return "D_DIV";
    }
    case Opcode::I_GT:
    {
        return "I_GT";
    }
    case Opcode::DI_GT:
    {
        return "DI_GT";
    }
    case Opcode::ID_GT:
    {
        return "ID_GT";
    }
    case Opcode::D_GT:
    {
        return "D_GT";
    }
    case Opcode::I_LT:
    {
        return "I_LT";
    }
    case Opcode::DI_LT:
    {
        return "DI_LT";
    }
    case Opcode::ID_LT:
    {
        return "ID_LT";
    }
    case Opcode::D_LT:
    {
        return "D_LT";
    }
    case Opcode::I_GEQ:
    {
        return "I_GEQ";
    }
    case Opcode::DI_GEQ:
    {
        return "DI_GEQ";
    }
    case Opcode::ID_GEQ:
    {
        return "ID_GEQ";
    }
    case Opcode::D_GEQ:
    {
        return "D_GEQ";
    }
    case Opcode::I_LEQ:
    {
        return "I_LEQ";
    }
    case Opcode::DI_LEQ:
    {
        return "DI_LEQ";
    }
    case Opcode::ID_LEQ:
    {
        return "ID_LEQ";
    }
    case Opcode::D_LEQ:
    {
        return "D_LEQ";
    }
    case Opcode::I_EQ_EQ:
    {
        return "I_EQ_EQ";
    }
    case Opcode::DI_EQ_EQ:
    {
        return "DI_EQ_EQ";
    }
    case Opcode::ID_EQ_EQ:
    {
        return "ID_EQ_EQ";
    }
    case Opcode::D_EQ_EQ:
    {
        return "D_EQ_EQ";
    }
    case Opcode::I_BANG_EQ:
    {
        return "I_BANG_EQ";
    }
    case Opcode::DI_BANG_EQ:
    {
        return "DI_BANG_EQ";
    }
    case Opcode::ID_BANG_EQ:
    {
        return "ID_BANG_EQ";
    }
    case Opcode::D_BANG_EQ:
    {
        return "D_BANG_EQ";
    }
    default:
    {
        return "UNRECOGNISED OPCODE " + std::to_string((uint8_t)o);
    }
    }
}

#define GET_TYPED_OP(l, op, r, ret) \
    do                              \
    {                               \
        if (l == 1 && r == 1)       \
            ret = Opcode::I_##op;   \
        else if (l == 1 && r == 2)  \
            ret = Opcode::ID_##op;  \
        else if (l == 2 && r == 1)  \
            ret = Opcode::DI_##op;  \
        else                        \
            ret = Opcode::D_##op;   \
    } while (false)

Opcode TokenToOpcode(TypeID l, TokenID t, TypeID r)
{
    Opcode o;
    if (t == TokenID::PLUS)
    {
        GET_TYPED_OP(l, ADD, r, o);
        return o;
    }
    else if (t == TokenID::MINUS)
    {
        GET_TYPED_OP(l, SUB, r, o);
        return o;
    }
    else if (t == TokenID::STAR)
    {
        GET_TYPED_OP(l, MUL, r, o);
        return o;
    }
    else if (t == TokenID::SLASH)
    {
        GET_TYPED_OP(l, DIV, r, o);
        return o;
    }
    else if (t == TokenID::GT)
    {
        GET_TYPED_OP(l, GT, r, o);
        return o;
    }
    else if (t == TokenID::LT)
    {
        GET_TYPED_OP(l, LT, r, o);
        return o;
    }
    else if (t == TokenID::GEQ)
    {
        GET_TYPED_OP(l, GEQ, r, o);
        return o;
    }
    else if (t == TokenID::LEQ)
    {
        GET_TYPED_OP(l, LEQ, r, o);
        return o;
    }
    else if (t == TokenID::EQ_EQ)
    {
        GET_TYPED_OP(l, EQ_EQ, r, o);
        return o;
    }
    else if (t == TokenID::BANG_EQ)
    {
        GET_TYPED_OP(l, BANG_EQ, r, o);
        return o;
    }
    else
        return Opcode::NONE;
}