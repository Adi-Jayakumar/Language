#pragma once
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
    Token prev;
    Token cur;
    std::string src;

    Lexer(const std::string &fPath);

    // lexes the token starting from where the current character is
    Token NextToken();
    // updates the previous and current tokens
    void Advance();

    void SkipWhiteSpace();
    Token LexDouble();
};