
#include "token.h"

std::ostream &operator<<(std::ostream &out, const TokenID &t)
{
    switch (t)
    {
    case TokenID::BEG:
    {
        out << "BEG";
        break;
    }
    case TokenID::INT_L:
    {
        out << "INT_L";
        break;
    }
    case TokenID::DOUBLE_L:
    {
        out << "DOUBLE_L";
        break;
    }
    case TokenID::BOOL_L:
    {
        out << "BOOL_L";
        break;
    }
    case TokenID::IDEN:
    {
        out << "IDEN";
        break;
    }
    case TokenID::TYPENAME:
    {
        out << "TYPENAME";
        break;
    }
    case TokenID::PLUS:
    {
        out << "PLUS";
        break;
    }
    case TokenID::MINUS:
    {
        out << "MINUS";
        break;
    }
    case TokenID::STAR:
    {
        out<< "STAR";
        break;
    }
    case TokenID::SLASH:
    {
        out << "SLASH";
        break;
    }
    case TokenID::GT:
    {
        out << "GT";
        break;
    }
    case TokenID::LT:
    {
        out << "LT";
        break;
    }
    case TokenID::GEQ:
    {
        out << "GEQ";
        break;
    }
    case TokenID::LEQ:
    {
        out << "LEQ";
        break;
    }
    case TokenID::EQ_EQ:
    {
        out << "EQ_EQ";
        break;
    }
    case TokenID::BANG_EQ:
    {
        out << "BANG_EQ";
        break;
    }
    case TokenID::EQ:
    {
        out << "EQ";
        break;
    }
    case TokenID::BANG:
    {
        out << "BANG";
        break;
    }
    case TokenID::SEMI:
    {
        out << "SEMI";
        break;
    }
    case TokenID::OPEN_PAR:
    {
        out << "OPEN_PAR";
        break;
    }
    case TokenID::CLOSE_PAR:
    {
        out << "CLOSE_PAR";
        break;
    }
    case TokenID::END:
    {
        out << "END";
        break;
    }
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const Token &t)
{
    out << "[Type: " << t.type << ", Literal: " << t.literal << ", Line: " << t.line << "]";
    return out;
}

bool IsLiteral(const Token &t)
{
    return (t.type == TokenID::INT_L) || (t.type == TokenID::DOUBLE_L) || (t.type == TokenID::BOOL_L);
}
