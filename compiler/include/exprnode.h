#pragma once
#include "common.h"
#include "token.h"
#include <cstring>
#include <memory>

class Compiler;
class StaticAnalyser;

class Expr
{
public:
    TypeData t = {0, false};
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeData Type(StaticAnalyser &t) = 0;
    // returns the type after typechecking is done
    virtual TypeData GetType() = 0;
    // compiles the node - implmented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
    // virtual bool IsTruthy() = 0;
    // virtual ~Expr() = 0;
};

std::ostream &operator<<(std::ostream &out, std::shared_ptr<Expr> &e);

class Literal : public Expr
{
public:
    Token loc;
    Literal(Token);
    // ~Literal() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class Unary : public Expr
{
public:
    Token op;
    std::shared_ptr<Expr> right;

    Unary(Token, std::shared_ptr<Expr>);
    // ~Unary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class Binary : public Expr
{
public:
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr>, Token, std::shared_ptr<Expr>);
    // ~Binary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class VarReference : public Expr
{
public:
    Token loc;
    std::string name;
    VarReference(Token);
    bool isArray = false;

    // ~VarReference() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class Assign : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> target;
    std::shared_ptr<Expr> val;

    Assign(std::shared_ptr<Expr>, std::shared_ptr<Expr>, Token);
    // ~Assign() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class FunctionCall : public Expr
{
public:
    Token loc;
    std::string name;
    std::vector<std::shared_ptr<Expr>> args;

    FunctionCall(std::string, std::vector<std::shared_ptr<Expr>>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

class ArrayIndex : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> name;
    std::shared_ptr<Expr> index;

    ArrayIndex(std::shared_ptr<Expr>, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

class BracedInitialiser : public Expr
{
public:
    Token loc;
    size_t size;
    std::vector<std::shared_ptr<Expr>> init;

    BracedInitialiser(size_t, std::vector<std::shared_ptr<Expr>>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

class DynamicAllocArray : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> size;

    DynamicAllocArray(TypeData, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

class FieldAccess : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> accessor;
    std::shared_ptr<Expr> accessee;

    FieldAccess(std::shared_ptr<Expr>, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

class TypeCast : public Expr
{
public:
    Token loc;
    TypeData type;
    std::shared_ptr<Expr> arg;
    bool isDownCast = false;

    TypeCast(TypeData, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};