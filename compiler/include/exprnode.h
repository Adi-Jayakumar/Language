#pragma once
#include "common.h"
#include "token.h"
#include <cstring>
#include <memory>

class ASTPrinter;
class Compiler;

enum class ExprKind
{
    LITERAL,
    UNARY,
    BINARY,
    VAR_REFERENCE,
    ASSIGN,
    FUNCTION_CALL,
    ARRAY_INDEX,
    BRACED_INITIALISER,
    DYNAMIC_ALLOC_ARRAY,
    FIELD_ACCESS,
    TYPE_CAST
};

class Expr
{
public:
    ExprKind kind;
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(ASTPrinter &p) = 0;
    // returns the type after typechecking is done
    virtual TypeData GetType() { return VOID_TYPE; };
    // compiles the node - implmented in Compiler.cpp
    virtual TypeData NodeCompile(Compiler &c) = 0;
    // virtual ~Expr() = 0;
};

class Literal : public Expr
{
public:
    TypeData t;
    Token loc;
    Literal(Token);

    Literal(int i) { loc.literal = std::to_string(i); };
    Literal(double d) { loc.literal = std::to_string(d); };
    Literal(bool b) { loc.literal = b ? "true" : "false"; };
    Literal(std::string s) { loc.literal = s; };
    Literal(char c) { loc.literal = std::string(&c, 1); };

    Literal(Token _loc, int i) : loc(_loc) { loc.literal = std::to_string(i); };
    Literal(Token _loc, double d) : loc(_loc) { loc.literal = std::to_string(d); };
    Literal(Token _loc, bool b) : loc(_loc) { loc.literal = b ? "true" : "false"; };
    Literal(Token _loc, std::string s) : loc(_loc) { loc.literal = s; };
    Literal(Token _loc, char c) : loc(_loc) { loc.literal = std::string(&c, 1); };

    // ~Literal() override = default;

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData GetType();
    TypeData NodeCompile(Compiler &c) override;
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
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
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
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
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
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
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
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class FunctionCall : public Expr
{
public:
    Token loc;
    std::string name;
    std::vector<TypeData> templates;
    std::vector<std::shared_ptr<Expr>> args;

    FunctionCall(std::string _name, std::vector<TypeData> _templates, std::vector<std::shared_ptr<Expr>> _args, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
};

class ArrayIndex : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> name;
    std::shared_ptr<Expr> index;

    ArrayIndex(std::shared_ptr<Expr> _name, std::shared_ptr<Expr> _index, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
};

class BracedInitialiser : public Expr
{
public:
    TypeData t;
    Token loc;
    size_t size;
    std::vector<std::shared_ptr<Expr>> init;

    BracedInitialiser(size_t _size, std::vector<std::shared_ptr<Expr>> _init, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData GetType();
    TypeData NodeCompile(Compiler &c) override;
};

class DynamicAllocArray : public Expr
{
public:
    TypeData t;
    Token loc;
    std::shared_ptr<Expr> size;

    DynamicAllocArray(TypeData _t, std::shared_ptr<Expr> _size, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData GetType();
    TypeData NodeCompile(Compiler &c) override;
};

class FieldAccess : public Expr
{
public:
    Token loc;
    std::shared_ptr<Expr> accessor;
    std::shared_ptr<Expr> accessee;

    FieldAccess(std::shared_ptr<Expr> _accessor, std::shared_ptr<Expr> _accessee, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
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
    void Print(ASTPrinter &p) override;
    TypeData NodeCompile(Compiler &c) override;
};