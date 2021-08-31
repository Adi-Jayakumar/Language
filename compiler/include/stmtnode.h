#pragma once
#include "exprnode.h"
#include <vector>

class PostConditionGenerator;

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
    virtual void GeneratePost(PostConditionGenerator &v) = 0;
};

class ExprStmt : public Stmt
{
public:
    SP<Expr> exp;
    ExprStmt(SP<Expr>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class DeclaredVar : public Stmt
{
public:
    TypeData t;
    std::string name;
    SP<Expr> value;
    DeclaredVar(TypeData, std::string, SP<Expr>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Block : public Stmt
{
public:
    std::vector<SP<Stmt>> stmts;
    Block(Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class IfStmt : public Stmt
{
public:
    SP<Expr> cond;
    SP<Stmt> thenBranch;
    SP<Stmt> elseBranch;
    IfStmt(SP<Expr>, SP<Stmt>, SP<Stmt>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class WhileStmt : public Stmt
{
public:
    SP<Expr> cond;
    SP<Stmt> body;
    Token loc;

    WhileStmt(SP<Expr>, SP<Stmt>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class FuncDecl : public Stmt
{
public:
    TypeData ret;
    std::string name;
    std::vector<std::pair<TypeData, std::string>> params;

    std::vector<SP<Stmt>> body;
    std::vector<SP<Expr>> preConds;

    std::vector<std::pair<TypeData, std::string>> templates;

    SP<Expr> postCond;

    FuncDecl(TypeData _ret,
             std::string &_name,
             std::vector<std::pair<TypeData, std::string>> &_params,
             std::vector<SP<Stmt>> &_body,
             std::vector<SP<Expr>> &_preConds,
             SP<Expr> &_postCond,
             Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Return : public Stmt
{
public:
    SP<Expr> retVal;

    Return(SP<Expr>, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class StructDecl : public Stmt
{
public:
    std::string name;
    TypeData parent = VOID_TYPE;
    std::vector<SP<Stmt>> decls;
    StructDecl(std::string &, TypeData &parent, std::vector<SP<Stmt>> &, Token);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
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
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Break : public Stmt
{
public:
    Break(Token);
    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Throw : public Stmt
{
public:
    SP<Expr> exp;
    Throw(SP<Expr> &, Token &);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class TryCatch : public Stmt
{
public:
    SP<Stmt> tryClause, catchClause;
    std::pair<TypeData, std::string> catchVar;

    TryCatch(SP<Stmt> &, SP<Stmt> &, std::pair<TypeData, std::string> &, Token &);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};