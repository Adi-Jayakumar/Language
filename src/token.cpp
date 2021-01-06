
#include "token.h"

std::ostream &operator<<(std::ostream &out, const TokenID &t)
{
    switch (t)
    {
    case TokenID::DOUBLE_L:
    {
        out << "DOUBLE_L";
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
