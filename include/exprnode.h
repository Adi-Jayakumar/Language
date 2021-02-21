#pragma once
#include "common.h"
#include "token.h"
#include <memory>

struct Compiler;
struct TypeChecker;

struct Expr
{
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
    TypeData t = {0, 0};
    Token loc;
    union combo
    {
        int i;
        double d;
        bool b;
    } as;
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
    TypeData t = {0, 0};

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
    TypeData t = {0, 0};

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
    TypeData t = {0, 0};
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
    TypeData t = {0, 0};

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
    TypeData t = {0, 0};

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
    TypeData t = {0, 0};

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
    TypeData t = {false, 0};

    InlineArray(size_t, std::vector<std::shared_ptr<Expr>> , Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeData Type(TypeChecker &t) override;
    TypeData GetType() override;
    void NodeCompile(Compiler &c) override;
};