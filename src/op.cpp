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
    case Opcode::DEL_V:
    {
        return "DEL_V";
    }
    case Opcode::VAR_D:
    {
        return "VAR_D";
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
    case Opcode::CALL_F:
    {
        return "CALL_F";
    }
    case Opcode::RETURN:
    {
        return "RETURN";
    }
    case Opcode::ADD:
    {
        return "ADD";
    }
    case Opcode::SUB:
    {
        return "SUB";
    }
    case Opcode::MUL:
    {
        return "MUL";
    }
    case Opcode::DIV:
    {
        return "DIV";
    }

    case Opcode::GT:
    {
        return "GT";
    }
    case Opcode::LT:
    {
        return "LT";
    }
    case Opcode::GEQ:
    {
        return "GEQ";
    }
    case Opcode::LEQ:
    {
        return "LEQ";
    }

    case Opcode::EQ_EQ:
    {
        return "EQ_EQ";
    }
    case Opcode::BANG_EQ:
    {
        return "BANG_EQ";
    }
    default:
    {
        return "UNRECOGNISED OPCODE " + std::to_string((uint16_t)o);
    }
    }
}

Opcode TokenToOpcode(TokenID t)
{
    if (t == TokenID::PLUS)
        return Opcode::ADD;
    else if (t == TokenID::MINUS)
        return Opcode::SUB;
    else if (t == TokenID::STAR)
        return Opcode::MUL;
    else if (t == TokenID::SLASH)
        return Opcode::DIV;
    else if (t == TokenID::GT)
        return Opcode::GT;
    else if (t == TokenID::LT)
        return Opcode::LT;
    else if (t == TokenID::GEQ)
        return Opcode::GEQ;
    else if (t == TokenID::LEQ)
        return Opcode::LEQ;
    else if (t == TokenID::EQ_EQ)
        return Opcode::EQ_EQ;
    else if (t == TokenID::BANG_EQ)
        return Opcode::BANG_EQ;
    else
        return Opcode::NONE;
}