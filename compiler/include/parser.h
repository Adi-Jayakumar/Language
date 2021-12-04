#pragma once
#include "lexer.h"
#include "stmtnode.h"
#include "symboltable.h"
#include <cassert>

struct Parser
{
    SymbolTable symbols;
    Lexer lex;
    Token prev;
    Token cur;
    Token next;

    size_t depth;
    bool hadError;

    Parser(const std::string &fPath, SymbolTable &_symbols);

    void ParseError(Token loc, std::string err);
    void PanicMode(std::initializer_list<TokenID> recovery);

    // updates the previous and current tokens
    void Advance();
    // checks t has same type as cur.type, gives an error
    void Check(TokenID t, std::string err);

    // parses a type eg. 'int', 'bool', 'Array<int>', etc...
    TypeData ParseType(std::string err);

    // parses a file into a list of statements
    std::vector<SP<Stmt>> Parse();

    std::vector<SP<Expr>> ParseVerCondition();

    // parses a block
    SP<Block> ParseBlock();
    // parses any declaration
    SP<Stmt> Declaration();
    // parses a variable declaraion
    SP<Stmt> VarDeclaration();
    // parses any statement
    SP<Stmt> Statement();
    // parses an if statement
    SP<Stmt> IfStatement();
    // parses a while statement
    SP<Stmt> WhileStatement();
    // parses a function declrataion
    SP<Stmt> FuncDeclaration();
    // parses a struct declaration
    SP<Stmt> ParseStructDecl();
    // parses a template function
    SP<Stmt> TemplateFunction();
    // parses an expression statement
    SP<Stmt> ExpressionStatement();
    // parses an import statement
    SP<Stmt> ParseImportStmt();
    std::vector<std::string> CommaSeparatedStrings();
    // parses any expression
    SP<Expr> Expression();
    // parses a sequence node
    SP<Expr> ParseSequenceNode();
    // parses an or expression
    SP<Expr> Or();
    // parses an and expression
    SP<Expr> And();
    // parses any field access
    SP<Expr> ParseFieldAccess();
    // parses an inline array
    SP<Expr> ParseBracedInitialiser();
    // parses any function call
    SP<Expr> FuncCall();
    // parses an array index expression
    SP<Expr> ParseArrayIndex(SP<Expr>);
    // parses any assignment
    SP<Expr> Assignment();
    // parses a series of == or != operations
    SP<Expr> EqualityCheck();
    // parses a series of >, <, >=, <= operations
    SP<Expr> Comparison();
    // parses a series of + or - operations
    SP<Expr> Sum();
    // parses a series of* or / operations
    SP<Expr> Product();
    // parses a series of - operations for the moment
    SP<Expr> UnaryOp();
    // parses an experession of the form '(' expression ')'
    SP<Expr> Grouping();
    // parses a literal -- just a double for now
    SP<Expr> LiteralNode();
};