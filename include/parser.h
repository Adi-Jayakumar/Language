#pragma once
#include "lexer.h"
#include "stmtnode.h"
#include <vector>

struct Parser
{
    Lexer lex;
    Token prev;
    Token cur;

    Parser(const std::string &fPath);

    // updates the previous and current tokens
    void Advance();
    // checks t has same type as cur.type, gives an error
    void Check(TokenID t, std::string err);
    // parses a file into a list of statements
    std::vector<Stmt *> Parse();
    // parses any declaration
    Stmt *Declaration();
    // parses a variable declaraion
    Stmt *VarDeclaration();
    // parses any statement
    Stmt *Statement();
    // parses an expression statement
    Stmt *ExpressionStatement();
    // parses any expression
    Expr *Expression();
    // parses any assignment
    Expr *Assignment();
    // parses a series of == or != operations
    Expr *EqualityCheck();
    // parses a series of >, <, >=, <= operations
    Expr *Comparison();
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