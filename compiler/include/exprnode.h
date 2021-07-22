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
    TypeData t = VOID_TYPE;
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeData Type(StaticAnalyser &t) = 0;
    // returns the type after typechecking is done
    virtual TypeData GetType() = 0;
    // compiles the node - implmented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
    virtual std::shared_ptr<Expr> Evaluate() = 0;
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
    std::shared_ptr<Expr> Evaluate() override;

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
    std::shared_ptr<Expr> Evaluate() override;

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
    std::shared_ptr<Expr> Evaluate() override;

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
    std::shared_ptr<Expr> Evaluate() override;

    // bool IsTruthy() override;
};

class Assign : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> target;
    std::shared_ptr<Expr> val;

    Assign(std::shared_ptr<Expr> _target, std::shared_ptr<Expr> _val, Token _loc);
    // ~Assign() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;

    // bool IsTruthy() override;
};

class FunctionCall : public Expr
{
public:
    Token loc;
    std::string name;
    std::vector<std::shared_ptr<Expr>> args;

    FunctionCall(std::string _name, std::vector<std::shared_ptr<Expr>> _args, Token _loc);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;
};

class ArrayIndex : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> name;
    std::shared_ptr<Expr> index;

    ArrayIndex(std::shared_ptr<Expr> _name, std::shared_ptr<Expr> _index, Token _loc);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;
};

class BracedInitialiser : public Expr
{
public:
    Token loc;
    size_t size;
    std::vector<std::shared_ptr<Expr>> init;

    BracedInitialiser(size_t _size, std::vector<std::shared_ptr<Expr>> _init, Token _loc);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;
};

class DynamicAllocArray : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> size;

    DynamicAllocArray(TypeData _t, std::shared_ptr<Expr> _size, Token _loc);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;
};

class FieldAccess : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> accessor;
    std::shared_ptr<Expr> accessee;

    FieldAccess(std::shared_ptr<Expr> _accessor, std::shared_ptr<Expr> _accessee, Token _loc);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;
};

class TypeCast : public Expr
{
public:
    Token loc;
    TypeData type;
    std::shared_ptr<Expr> arg;
    bool isDownCast = false;

    TypeCast(TypeData _type, std::shared_ptr<Expr> _arg, Token _loc);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(StaticAnalyser &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    std::shared_ptr<Expr> Evaluate() override;
};