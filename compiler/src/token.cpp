
#include "token.h"

std::string ToString(const TokenID &t)
{
    switch (t)
    {
    case TokenID::BEG:
    {
        return "BEG";
    }
    case TokenID::INT_L:
    {
        return "INT_L";
    }
    case TokenID::DOUBLE_L:
    {
        return "DOUBLE_L";
    }
    case TokenID::BOOL_L:
    {
        return "BOOL_L";
    }
    case TokenID::STRING_L:
    {
        return "STRING_L";
    }
    case TokenID::CHAR_L:
    {
        return "CHAR_L";
    }
    case TokenID::IDEN:
    {
        return "IDEN";
    }
    case TokenID::FUNC:
    {
        return "FUNC";
    }
    case TokenID::CAST:
    {
        return "CAST";
    }
    case TokenID::THROW:
    {
        return "THROW";
    }
    case TokenID::TRY:
    {
        return "TRY";
    }
    case TokenID::CATCH:
    {
        return "CATCH";
    }
    case TokenID::RETURN:
    {
        return "RETURN";
    }
    case TokenID::BREAK:
    {
        return "BREAK";
    }
    case TokenID::COMMA:
    {
        return "COMMA";
    }
    case TokenID::DOT:
    {
        return "DOT";
    }
    case TokenID::NULL_T:
    {
        return "NULL_T";
    }
    case TokenID::ARRAY:
    {
        return "ARRAY";
    }
    case TokenID::WHILE:
    {
        return "WHILE";
    }
    case TokenID::STRUCT:
    {
        return "STRUCT";
    }
    case TokenID::COLON:
    {
        return "COLON";
    }
    case TokenID::IMPORT:
    {
        return "IMPORT";
    }
    case TokenID::FROM:
    {
        return "FROM";
    }
    case TokenID::TYPENAME:
    {
        return "TYPENAME";
    }
    case TokenID::TYPENAME_KW:
    {
        return "TYPENAME_KW";
    }
    case TokenID::TEMPLATE:
    {
        return "TEMPLATE";
    }
    case TokenID::PLUS:
    {
        return "PLUS";
    }
    case TokenID::MINUS:
    {
        return "MINUS";
    }
    case TokenID::STAR:
    {
        return "STAR";
    }
    case TokenID::SLASH:
    {
        return "SLASH";
    }
    case TokenID::GT:
    {
        return "GT";
    }
    case TokenID::LT:
    {
        return "LT";
    }
    case TokenID::GEQ:
    {
        return "GEQ";
    }
    case TokenID::LEQ:
    {
        return "LEQ";
    }
    case TokenID::EQ_EQ:
    {
        return "EQ_EQ";
    }
    case TokenID::BANG_EQ:
    {
        return "BANG_EQ";
    }
    case TokenID::AND_AND:
    {
        return "AND_AND";
    }
    case TokenID::OR_OR:
    {
        return "OR_OR";
    }
    case TokenID::EQ:
    {
        return "EQ";
    }
    case TokenID::BANG:
    {
        return "BANG";
    }
    case TokenID::SEMI:
    {
        return "SEMI";
    }
    case TokenID::OPEN_PAR:
    {
        return "OPEN_PAR";
    }
    case TokenID::CLOSE_PAR:
    {
        return "CLOSE_PAR";
    }
    case TokenID::OPEN_VER:
    {
        return "OPEN_VER";
    }
    case TokenID::CLOSE_VER:
    {
        return "CLOSE_VER";
    }
    case TokenID::RESULT:
    {
        return "RESULT";
    }
    case TokenID::SEQUENCE:
    {
        return "SEQUENCE";
    }
    case TokenID::OPEN_BRACE:
    {
        return "OPEN_BRACE";
    }
    case TokenID::CLOSE_BRACE:
    {
        return "CLOSE_BRACE";
    }
    case TokenID::OPEN_SQ:
    {
        return "OPEN_SQ";
    }
    case TokenID::CLOSE_SQ:
    {
        return "CLOSE_SQ";
    }
    case TokenID::IF:
    {
        return "IF";
    }
    case TokenID::ELSE:
    {
        return "ELSE";
    }
    case TokenID::END:
    {
        return "END";
    }
    default:
    {
        return "UNRECOGNISED TOKEN";
    }
    }
}

std::ostream &operator<<(std::ostream &out, const TokenID &t)
{
    out << ToString(t);
    return out;
}

std::ostream &operator<<(std::ostream &out, const Token &t)
{
    out << "[Type: " << t.type << ", Literal: " << t.literal << ", Line: " << t.line << "]";
    return out;
}

bool IsLiteral(const Token &t)
{
    return (t.type == TokenID::INT_L) || (t.type == TokenID::DOUBLE_L) || (t.type == TokenID::BOOL_L) || (t.type == TokenID::STRING_L) || (t.type == TokenID::CHAR_L);
}
