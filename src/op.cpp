#include "op.h"
#include "common.h"

std::string ToString(Opcode o)
{
    switch (o)
    {
    case Opcode::POP:
    {
        return "POP";
    }
    case Opcode::LOAD_INT:
    {
        return "LOAD_INT";
    }
    case Opcode::LOAD_DOUBLE:
    {
        return "LOAD_DOUBLE";
    }
    case Opcode::LOAD_BOOL:
    {
        return "LOAD_BOOL";
    }
    case Opcode::LOAD_STRING:
    {
        return "LOAD_STRING";
    }
    case Opcode::LOAD_CHAR:
    {
        return "LOAD_CHAR";
    }
    case Opcode::VAR_A:
    {
        return "VAR_A";
    }
    case Opcode::VAR_A_GLOBAL:
    {
        return "VAR_A_GLOBAL";
    }
    case Opcode::VAR_D_GLOBAL:
    {
        return "VAR_D_GLOBAL";
    }
    case Opcode::GET_V:
    {
        return "GET_V";
    }
    case Opcode::GET_V_GLOBAL:
    {
        return "GET_V_GLOBAL";
    }
    case Opcode::ARR_D:
    {
        return "ARR_D";
    }
    case Opcode::ARR_INDEX:
    {
        return "ARR_INDEX";
    }
    case Opcode::ARR_SET:
    {
        return "ARR_SET";
    }
    case Opcode::ARR_ALLOC:
    {
        return "ARR_ALLOC";
    }
    case Opcode::STRUCT_ALLOC:
    {
        return "STRUCT_ALLOC";
    }
    case Opcode::STRING_INDEX:
    {
        return "STRING_INDEX";
    }
    case Opcode::STRING_SET:
    {
        return "STRING_SET";
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
    case Opcode::NATIVE_CALL:
    {
        return "NATIVE_CALL";
    }
    case Opcode::PRINT:
    {
        return "PRINT";
    }
    case Opcode::STRUCT_MEMBER:
    {
        return "STRUCT_MEMBER";
    }
    case Opcode::STRUCT_D:
    {
        return "STRUCT_D";
    }
    case Opcode::STRUCT_MEMBER_SET:
    {
        return "STRUCT_MEMBER_SET";
    }
    case Opcode::CAST:
    {
        return "CAST";
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
    case Opcode::S_ADD:
    {
        return "S_ADD";
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
    case Opcode::N_EQ_EQ:
    {
        return "N_EQ_EQ";
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
    case Opcode::B_EQ_EQ:
    {
        return "B_BANG_EQ";
    }
    case Opcode::N_BANG_EQ:
    {
        return "N_BANG_EQ";
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
    case Opcode::B_BANG_EQ:
    {
        return "B_BANG_EQ";
    }
    case Opcode::BANG:
    {
        return "BANG";
    }
    default:
    {
        return "UNRECOGNISED OPCODE " + std::to_string((uint8_t)o);
    }
    }
}