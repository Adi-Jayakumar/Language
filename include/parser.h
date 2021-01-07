#pragma once
#include "exprnode.h"
#include "lexer.h"

struct Parser
{
    Lexer lex;
    Token prev;
    Token cur;

    Parser(const std::string &fPath);

    // updates the previous and current tokens
    void Advance();

    // parses a sum
    Expr *Sum();
    Expr *Product();
    Expr *UnaryOp();
    Expr *Grouping();
    Expr *LiteralNode();
};