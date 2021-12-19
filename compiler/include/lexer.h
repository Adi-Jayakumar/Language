#pragma once
#include "perror.h"
#include "symboltable.h"
#include "token.h"
#include "typedata.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace IO
{
    std::string GetSrcString(std::string fName);
} // namespace IO

class Lexer
{
public:
    size_t index;
    size_t line;
    std::string src;
    SymbolTable *symbols;

    Lexer(const std::string &fPath, SymbolTable *_symbols);

    size_t LineSize();

    void LexError(std::string msg);

    // lexes the token starting from where the current character is
    Token NextToken();

    void SkipWhiteSpace();
    void SkipComment();
    Token LexLiteral();

    Token LexNumber();
    bool CheckKeyword(Token &tok);
    bool MatchKeyWord(std::string kw, TokenID t, Token &tok);
};