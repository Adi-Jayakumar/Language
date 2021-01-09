#pragma once
#include "token.h"
#include <fstream>
#include <streambuf>
#include <string>
#include "perror.h"

namespace IO
{
    std::string GetSrcString(std::string fName);
} // namespace IO

struct Lexer
{
    size_t index;
    size_t line;
    std::string src;

    Lexer(){};
    Lexer(const std::string &fPath);

    size_t LineSize();

    // lexes the token starting from where the current character is
    Token NextToken();

    void SkipWhiteSpace();
    Token LexLiteral();
    
    Token LexNumber();
    bool CheckKeyword(Token &tok);
    bool MatchKeyWord(std::string kw, TokenID t, Token &tok);
};