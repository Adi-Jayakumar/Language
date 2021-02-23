#pragma once
#include "common.h"
#include "token.h"
#include <memory>
#include <cstring>

struct Compiler;
struct TypeChecker;

struct Expr
{
    TypeData t = {false, 0};
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(std::ostream &out) = 0;
    // returns the type of the node - implemented in typechecker.cpp
    virtual TypeData Type(TypeChecker &t) = 0;
    // returns the type after typechecking is done
    virtual TypeData GetType() = 0;
    // compiles the node - implmented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
    // virtual bool IsTruthy() = 0;
    // virtual ~Expr() = 0;
};

std::ostream &operator<<(std::ostream &out, std::shared_ptr<Expr> &e);

/*
    TypeIDs are represented as TypeID with
    1 ---> int
    2 ---> double
    3 ---> bool
*/
struct Literal : Expr
{
    Token loc;
    Literal(Token);
    // ~Literal() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct Unary : Expr
{
    Token op;
    std::shared_ptr<Expr> right;

    Unary(Token, std::shared_ptr<Expr>);
    // ~Unary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct Binary : Expr
{
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr>, Token, std::shared_ptr<Expr>);
    // ~Binary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct VarReference : Expr
{
    Token loc;
    std::string name;
    VarReference(Token);
    bool isArray = false;

    // ~VarReference() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct Assign : Expr
{
    Token loc;
    std::shared_ptr<Expr> target;
    std::shared_ptr<Expr> val;

    Assign(std::shared_ptr<Expr>, std::shared_ptr<Expr>, Token);
    // ~Assign() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct FunctionCall : Expr
{
    Token loc;
    std::string name;
    std::vector<std::shared_ptr<Expr>> args;

    FunctionCall(std::string, std::vector<std::shared_ptr<Expr>>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

struct ArrayIndex : Expr
{
    Token loc;
    std::string name;
    std::shared_ptr<Expr> index;

    ArrayIndex(std::string, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

struct InlineArray : Expr
{
    Token loc;
    size_t size;
    std::vector<std::shared_ptr<Expr>> init;

    InlineArray(size_t, std::vector<std::shared_ptr<Expr>>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};

struct DynamicAllocArray : Expr
{
    Token loc;
    std::shared_ptr<Expr> size;

    DynamicAllocArray(TypeData, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};