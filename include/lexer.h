#pragma once
#include "perror.h"
#include "token.h"
#include <fstream>
#include <streambuf>
#include <string>
#include <unordered_map>

namespace IO
{
    std::string GetSrcString(std::string fName);
} // namespace IO

// map from string typenames to uint8_t type ids.
static std::unordered_map<std::string, uint8_t> TypeNameMap{{"int", 1}, {"double", 2}, {"bool", 3}};

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