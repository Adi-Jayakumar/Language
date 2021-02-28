#pragma once
#include "common.h"
#include "perror.h"
#include "token.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace IO
{
    std::string GetSrcString(std::string fName);
} // namespace IO

struct Lexer
{
    size_t index;
    size_t line;
    std::string src;

    TypeID NumTypes = 5;

    Lexer(){};
    Lexer(const std::string &fPath);

    size_t LineSize();

    void LexError(std::string msg);

    // lexes the token starting from where the current character is
    Token NextToken();

    void SkipWhiteSpace(size_t &i);
    Token LexLiteral();

    Token LexNumber();
    bool CheckKeyword(Token &tok);
    bool MatchKeyWord(std::string kw, TokenID t, Token &tok);
};