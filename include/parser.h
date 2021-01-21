#pragma once
#include "lexer.h"
#include "stmtnode.h"

struct Parser
{
    Lexer lex;
    Token prev;
    Token cur;
    Token next;

    uint16_t depth;

    Parser(const std::string &fPath);

    void ParseError(Token loc, std::string err);

    // updates the previous and current tokens
    void Advance();
    // checks t has same type as cur.type, gives an error
    void Check(TokenID t, std::string err);
    // parses a file into a list of statements
    std::shared_ptr<Block> ParseBlock();
    // parses any declaration
    std::shared_ptr<Stmt> Declaration();
    // parses a variable declaraion
    std::shared_ptr<Stmt> VarDeclaration();
    // parses any statement
    std::shared_ptr<Stmt> Statement();
    // parses an if statement
    std::shared_ptr<Stmt> IfStatement();
    // parses a function Declrataion
    std::shared_ptr<Stmt> FuncDeclaration();
    // parses an expression statement
    std::shared_ptr<Stmt> ExpressionStatement();
    // parses any expression
    std::shared_ptr<Expr> Expression();
    // parses any function call
    std::shared_ptr<Expr> FuncCall();
    // parses any assignment
    std::shared_ptr<Expr> Assignment();
    // parses a series of == or != operations
    std::shared_ptr<Expr> EqualityCheck();
    // parses a series of >, <, >=, <= operations
    std::shared_ptr<Expr> Comparison();
    // parses a series of + or - operations
    std::shared_ptr<Expr> Sum();
    // parses a series of* or / operations
    std::shared_ptr<Expr> Product();
    // parses a series of - operations for the moment
    std::shared_ptr<Expr> UnaryOp();
    // parses an experession of the form '(' expression ')'
    std::shared_ptr<Expr> Grouping();
    // parses a literal -- just a double for now
    std::shared_ptr<Expr> LiteralNode();
};