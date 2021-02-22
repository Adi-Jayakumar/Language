#include "lexer.h"

std::string IO::GetSrcString(std::string fName)
{
    std::ifstream in = std::ifstream(fName);
    std::string src = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return src;
}

Lexer::Lexer(const std::string &fPath)
{
    index = 0;
    line = 1;
    src = IO::GetSrcString(fPath);
}

size_t Lexer::LineSize()
{
    size_t l = 0;
    size_t i = index;
    while (i != src.length() && src[i] != '\n')
    {
        l++;
        i++;
    }
    return l;
}

Token Lexer::NextToken()
{
    SkipWhiteSpace();

    if (index == src.length())
        return {TokenID::END, "", line};

    if (isdigit(src[index]))
        return LexNumber();

    if (isalpha(src[index]))
    {
        Token t;
        if (CheckKeyword(t))
        {
            index += t.literal.length();
            return t;
        }
        else
        {
            std::string name;
            while (isalnum(src[index]))
            {
                name += src[index];
                index++;
            }
            // used for when custom types are added in the form of classes
            if (TypeNameMap.find(name) != TypeNameMap.end())
                return {TokenID::TYPENAME, name, line};
            else
                return {TokenID::IDEN, name, line};
        }
    }

    Token res;

    switch (src[index])
    {
    case '+':
    {
        res = {TokenID::PLUS, "+", line};
        break;
    }
    case '-':
    {
        res = {TokenID::MINUS, "-", line};
        break;
    }
    case '*':
    {
        res = {TokenID::STAR, "*", line};
        break;
    }
    case '/':
    {
        res = {TokenID::SLASH, "/", line};
        break;
    }
    case '>':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::GEQ, ">=", line};
            index++;
        }
        else
            res = {TokenID::GT, ">", line};
        break;
    }
    case '<':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::LEQ, "<=", line};
            index++;
        }
        else
            res = {TokenID::LT, "<", line};
        break;
    }
    case '=':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::EQ_EQ, "==", line};
            index++;
        }
        else
            res = {TokenID::EQ, "=", line};
        break;
    }
    case '!':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::BANG_EQ, "!=", line};
            index++;
        }
        else
            res = {TokenID::BANG, "!", line};
        break;
    }
    case ';':
    {
        res = {TokenID::SEMI, ";", line};
        break;
    }
    case '(':
    {
        res = {TokenID::OPEN_PAR, "(", line};
        break;
    }
    case ')':
    {
        res = {TokenID::CLOSE_PAR, ")", line};
        break;
    }
    case '{':
    {
        res = {TokenID::OPEN_BRACE, "{", line};
        break;
    }
    case '}':
    {
        res = {TokenID::CLOSE_BRACE, "}", line};
        break;
    }
    case '[':
    {
        res = {TokenID::OPEN_SQ, "[", line};
        break;
    }
    case ']':
    {
        res = {TokenID::CLOSE_SQ, "]", line};
        break;
    }
    case ',':
    {
        res = {TokenID::COMMA, ",", line};
        break;
    }
    default:
    {
        size_t lineSize = LineSize();
        Error e = Error("[LEX ERROR]: Unkown token on line " + std::to_string(line) + "\nNear:\n '" + src.substr(index, index + std::min(5U, (unsigned)lineSize)) + "'");
        e.Dump();
        break;
    }
    }

    index++;
    return res;
}

void Lexer::SkipWhiteSpace()
{
    while (isspace(src[index]))
    {
        if (src[index] == '\n')
            line++;
        index++;
    }
}

Token Lexer::LexNumber()
{
    int start = index;
    int length = 0;

    bool hadDot = false;

    while (isdigit(src[index]) || src[index] == '.')
    {
        if (src[index] == '.')
            hadDot = true;
        index++;
        length++;
    }

    return {hadDot ? TokenID::DOUBLE_L : TokenID::INT_L, src.substr(start, length), line};
}

bool Lexer::CheckKeyword(Token &tok)
{
    switch (src[index])
    {
    case 'A':
    {
        return MatchKeyWord("rray", TokenID::ARRAY, tok);
    }
    case 'b':
    {
        return MatchKeyWord("ool", TokenID::TYPENAME, tok);
    }
    case 'd':
    {
        return MatchKeyWord("ouble", TokenID::TYPENAME, tok);
    }
    case 'e':
    {
        return MatchKeyWord("lse", TokenID::ELSE, tok);
    }
    case 'f':
    {
        return MatchKeyWord("unction", TokenID::FUNC, tok) || MatchKeyWord("alse", TokenID::BOOL_L, tok);
    }
    case 'i':
    {
        return MatchKeyWord("nt", TokenID::TYPENAME, tok) || MatchKeyWord("f", TokenID::IF, tok);
    }
    case 'r':
    {
        return MatchKeyWord("eturn", TokenID::RETURN, tok);
    }
    case 't':
    {
        return MatchKeyWord("rue", TokenID::BOOL_L, tok);
    }
    case 'v':
    {
        return MatchKeyWord("oid", TokenID::TYPENAME, tok);
    }
    case 'w':
    {
        return MatchKeyWord("hile", TokenID::WHILE, tok);
    }
    }
    return false;
}

bool Lexer::MatchKeyWord(std::string kw, TokenID t, Token &tok)
{
    std::string candidate = src.substr(index + 1, kw.length());
    if (candidate == kw)
    {
        tok = {t, src[index] + candidate, line};
        return true;
    }
    else
        return false;
}