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

    // parses a series of + or - operations
    Expr *Sum();
    // parses a series of * or / operations
    Expr *Product();
    // parses a series of - operations for the moment
    Expr *UnaryOp();
    // parses an experession of the form '(' expression ')'
    Expr *Grouping();
    // parses a literal -- just a double for now
    Expr *LiteralNode();
};