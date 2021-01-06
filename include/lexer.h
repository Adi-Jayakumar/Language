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
    std::string src;

    Lexer(const std::string &fPath);

    // parses the token starting from where the current character is
    Token Advance();

    void SkipWhiteSpace();
    Token LexDouble();
};