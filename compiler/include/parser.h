#pragma once
#include "lexer.h"
#include "stmtnode.h"
#include <cassert>

struct Parser
{
    Lexer lex;
    Token prev;
    Token cur;
    Token next;

    size_t numTypes = NUM_DEF_TYPES;

    uint8_t depth = 1;
    bool hadError = false;

    Parser(const std::string &fPath);

    void ParseError(Token loc, std::string err);
    void PanicMode(std::initializer_list<TokenID> recovery);

    // updates the previous and current tokens
    void Advance();
    // checks t has same type as cur.type, gives an error
    void Check(TokenID t, std::string err);

    // parses a type eg. 'int', 'bool', 'Array<int>', etc...
    TypeData ParseType(std::string err);

    // parses a file into a list of statements
    std::vector<std::shared_ptr<Stmt>> Parse();

    std::vector<std::shared_ptr<Expr>> ParseVerCondition();

    // parses a block
    std::shared_ptr<Block> ParseBlock();
    // parses any declaration
    std::shared_ptr<Stmt> Declaration();
    // parses a variable declaraion
    std::shared_ptr<Stmt> VarDeclaration();
    // parses any statement
    std::shared_ptr<Stmt> Statement();
    // parses an if statement
    std::shared_ptr<Stmt> IfStatement();
    // parses a while statement
    std::shared_ptr<Stmt> WhileStatement();
    // parses a function declrataion
    std::shared_ptr<Stmt> FuncDeclaration();
    // parses a struct declaration
    std::shared_ptr<Stmt> ParseStructDecl();
    // parses a template function
    std::shared_ptr<Stmt> TemplateFunction();
    // parses an expression statement
    std::shared_ptr<Stmt> ExpressionStatement();
    // parses an import statement
    std::shared_ptr<Stmt> ParseImportStmt();
    std::vector<std::string> CommaSeparatedStrings();
    // parses any expression
    std::shared_ptr<Expr> Expression();
    // parses a sequence node
    std::shared_ptr<Expr> ParseSequenceNode();
    // parses an or expression
    std::shared_ptr<Expr> Or();
    // parses an and expression
    std::shared_ptr<Expr> And();
    // parses any field access
    std::shared_ptr<Expr> ParseFieldAccess();
    // parses an inline array
    std::shared_ptr<Expr> ParseBracedInitialiser();
    // parses any function call
    std::shared_ptr<Expr> FuncCall();
    // parses an array index expression
    std::shared_ptr<Expr> ParseArrayIndex(std::shared_ptr<Expr>);
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