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

Token Lexer::Advance()
{
    
    if(index == src.length())
        return {TokenID::END, "", line};

    SkipWhiteSpace();

    if (isdigit(src[index]))
        return LexDouble();

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

Token Lexer::LexDouble()
{
    int start = index;
    int length = 0;

    while (isdigit(src[index]) || src[index] == '.')
    {
        index++;
        length++;
    }

    return {TokenID::DOUBLE_L, src.substr(start, length), line};
}