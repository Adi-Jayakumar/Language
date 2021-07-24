#pragma once
#include "exprnode.h"
#include <vector>

class Stmt
{
public:
    Token loc;
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual void Type(StaticAnalyser &t) = 0;
    // compiles the node - implemented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
    virtual void Evaluate() = 0;
    virtual void Propagate(ConstantPropagator &cp) = 0;
};

std::ostream &operator<<(std::ostream &out, Stmt *s);

class ExprStmt : public Stmt
{
public:
    std::shared_ptr<Expr> exp;
    ExprStmt(std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class DeclaredVar : public Stmt
{
public:
    TypeData t;
    std::string name;
    std::shared_ptr<Expr> value;
    DeclaredVar(TypeData, std::string, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class Block : public Stmt
{
public:
    uint8_t depth;
    std::vector<std::shared_ptr<Stmt>> stmts;
    Block(uint8_t, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class IfStmt : public Stmt
{
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
    IfStmt(std::shared_ptr<Expr>, std::shared_ptr<Stmt>, std::shared_ptr<Stmt>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class WhileStmt : public Stmt
{
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> body;
    Token loc;

    WhileStmt(std::shared_ptr<Expr>, std::shared_ptr<Stmt>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class FuncDecl : public Stmt
{
public:
    TypeData ret;
    std::string name;
    // TODO - Convert to vector of pairs
    std::vector<TypeData> argtypes;
    std::vector<std::string> paramIdentifiers;

    std::vector<std::shared_ptr<Stmt>> body;

    std::vector<std::shared_ptr<Expr>> preConds;

    // FuncDecl(TypeData, std::string &, std::vector<TypeData> &, std::vector<std::string> &, std::vector<std::shared_ptr<Stmt>> &, Token);
    FuncDecl(TypeData, std::string &, std::vector<TypeData> &, std::vector<std::string> &, std::vector<std::shared_ptr<Stmt>> &, std::vector<std::shared_ptr<Expr>> &, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class Return : public Stmt
{
public:
    std::shared_ptr<Expr> retVal;
    std::vector<std::shared_ptr<Expr>> postConds;

    Return(std::shared_ptr<Expr>, Token);
    Return(std::shared_ptr<Expr>, std::vector<std::shared_ptr<Expr>> &, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class StructDecl : public Stmt
{
public:
    std::string name;
    TypeData parent;
    std::vector<std::shared_ptr<Stmt>> decls;
    StructDecl(std::string &, TypeData &parent, std::vector<std::shared_ptr<Stmt>> &, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
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
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class Break : public Stmt
{
public:
    Break(Token);
    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class Throw : public Stmt
{
public:
    std::shared_ptr<Expr> exp;
    Throw(std::shared_ptr<Expr> &, Token &);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};

class TryCatch : public Stmt
{
public:
    std::shared_ptr<Stmt> tryClause, catchClause;
    std::pair<TypeData, std::string> catchVar;

    TryCatch(std::shared_ptr<Stmt> &, std::shared_ptr<Stmt> &, std::pair<TypeData, std::string> &, Token &);

    Token Loc() override;
    void Print(std::ostream &out) override;
    void Type(StaticAnalyser &t) override;
    void NodeCompile(Compiler &c) override;
    void Evaluate() override;
    void Propagate(ConstantPropagator &cp) override;
};