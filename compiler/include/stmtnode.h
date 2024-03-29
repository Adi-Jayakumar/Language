#pragma once
#include "exprnode.h"
#include <vector>

enum class StmtKind
{
    EXPR_STMT,
    DECLARED_VAR,
    BLOCK,
    IF_STMT,
    WHILE_STMT,
    FUNC_DECL,
    RETURN,
    STRUCT_DECL,
    IMPORT_STMT,
    BREAK,
    THROW,
    TRY_CATCH
};

class Stmt
{
public:
    StmtKind kind;
    Token loc;
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(ASTPrinter &p) = 0;
    virtual void Analyse(StaticAnalyser &sa) = 0;
    // compiles the node - implemented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
};

class ExprStmt : public Stmt
{
public:
    std::shared_ptr<Expr> exp;
    ExprStmt(std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class DeclaredVar : public Stmt
{
public:
    TypeData t;
    std::string name;
    std::shared_ptr<Expr> value;
    DeclaredVar(TypeData, std::string, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class Block : public Stmt
{
public:
    uint8_t depth;
    std::vector<std::shared_ptr<Stmt>> stmts;
    Block(uint8_t, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class IfStmt : public Stmt
{
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
    IfStmt(std::shared_ptr<Expr>, std::shared_ptr<Stmt>, std::shared_ptr<Stmt>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class WhileStmt : public Stmt
{
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> body;
    Token loc;

    WhileStmt(std::shared_ptr<Expr>, std::shared_ptr<Stmt>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class FuncDecl : public Stmt
{
public:
    TypeData ret;
    std::string name;
    std::vector<std::pair<TypeData, std::string>> params;

    std::vector<std::shared_ptr<Stmt>> body;
    std::vector<std::shared_ptr<Expr>> preConds;

    std::vector<std::pair<TypeData, std::string>> templates;

    std::shared_ptr<Expr> postCond;

    FuncDecl(TypeData _ret,
             std::string &_name,
             std::vector<std::pair<TypeData, std::string>> &_params,
             std::vector<std::shared_ptr<Stmt>> &_body,
             std::vector<std::shared_ptr<Expr>> &_preConds,
             std::shared_ptr<Expr> &_postCond,
             Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class Return : public Stmt
{
public:
    std::shared_ptr<Expr> retVal;

    Return(std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class StructDecl : public Stmt
{
public:
    std::string name;
    TypeData parent = VOID_TYPE;
    std::vector<std::shared_ptr<Stmt>> decls;
    StructDecl(std::string &, TypeData &parent, std::vector<std::shared_ptr<Stmt>> &, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

/*
import math, physics
from latin import nominative, accusative
i.e. import --> comma seperated list of libraries <---- imports everything
i.e. from --> single module name import comma seperated list of symbols <---- only imports those symbols
*/
class ImportStmt : public Stmt
{
public:
    std::vector<std::string> libraries;
    ImportStmt(std::vector<std::string> &, Token &);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class Break : public Stmt
{
public:
    Break(Token);
    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class Throw : public Stmt
{
public:
    std::shared_ptr<Expr> exp;
    Throw(std::shared_ptr<Expr> &, Token &);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};

class TryCatch : public Stmt
{
public:
    std::shared_ptr<Stmt> tryClause, catchClause;
    std::pair<TypeData, std::string> catchVar;

    TryCatch(std::shared_ptr<Stmt> &, std::shared_ptr<Stmt> &, std::pair<TypeData, std::string> &, Token &);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
};