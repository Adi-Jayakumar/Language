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
    virtual TypeID Type(TypeChecker &t) = 0;
    // returns the type after typechecking is done
    virtual TypeID GetType() = 0;
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
    TypeID typeID = 0;
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
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct Unary : Expr
{
    Token op;
    std::shared_ptr<Expr> right;
    TypeID typeID = 0;

    Unary(Token, std::shared_ptr<Expr>);
    // ~Unary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct Binary : Expr
{
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
    TypeID typeID = 0;

    Binary(std::shared_ptr<Expr>, Token, std::shared_ptr<Expr>);
    // ~Binary() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct VarReference : Expr
{
    Token loc;
    std::string name;
    VarReference(Token);
    TypeID typeID = 0;
    bool isArray = false;

    // ~VarReference() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct Assign : Expr
{
    Token loc;
    std::shared_ptr<VarReference> var;
    std::shared_ptr<Expr> val;
    TypeID typeID = 0;

    Assign(std::shared_ptr<VarReference>, std::shared_ptr<Expr>, Token);
    // ~Assign() override = default;

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

struct FunctionCall : Expr
{
    Token loc;
    std::string name;
    std::vector<std::shared_ptr<Expr>> args;
    TypeID typeID = 0;

    FunctionCall(std::string, std::vector<std::shared_ptr<Expr>>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
};

struct ArrayIndex : Expr
{
    Token loc;
    std::string name;
    std::shared_ptr<Expr> index;
    TypeID typeID = 0;

    ArrayIndex(std::string, std::shared_ptr<Expr>, Token);

    Token Loc() override;
    void Print(std::ostream &out) override;
    TypeID Type(TypeChecker &t) override;
    TypeID GetType() override;
    void NodeCompile(Compiler &c) override;
};