#pragma once
#include "common.h"
#include "token.h"
#include <cstring>
#include <memory>
#include <z3++.h>

template <class T>
using SP = std::shared_ptr<T>;

class ASTPrinter;
class Compiler;
class StaticAnalyser;
class Verifier;

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
    TYPE_CAST,
    SEQUENCE
};

// TODO - Change explicit constructors to initalisers
// and change to pass by reference instead of value

class Expr
{
public:
    ExprKind kind;
    TypeData t;
    virtual Token Loc() = 0;
    TypeData GetType() { return t; }
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(ASTPrinter &p) = 0;
    // ensures that the node is a valid node or throws error
    virtual TypeData Analyse(StaticAnalyser &sa) = 0;
    // compiles the node - implmented in compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
    // generates a postcondition from the node - implemented in postcondition.cpp
    virtual z3::expr CreateZ3Expr(Verifier &v) = 0;
    // virtual ~Expr() = 0;
};

class Literal : public Expr
{
public:
    Token loc;
    Literal(const Token &_loc)
    {
        kind = ExprKind::LITERAL;
        loc = _loc;
        if (loc.type == TokenID::INT_L)
            t = INT_TYPE;
        else if (loc.type == TokenID::DOUBLE_L)
            t = DOUBLE_TYPE;
        else if (loc.type == TokenID::BOOL_L)
            t = BOOL_TYPE;
        else if (loc.type == TokenID::STRING_L)
            t = STRING_TYPE;
        else
            t = CHAR_TYPE;
    };

    Literal(int i)
    {
        kind = ExprKind::LITERAL;
        t = INT_TYPE;
        loc.literal = std::to_string(i);
    };

    Literal(double d)
    {
        t = DOUBLE_TYPE;
        kind = ExprKind::LITERAL;
        loc.literal = std::to_string(d);
    };

    Literal(bool b)
    {
        t = BOOL_TYPE;
        kind = ExprKind::LITERAL;
        loc.literal = b ? "true" : "false";
    };

    Literal(std::string s) { kind = ExprKind::LITERAL,
                             t = STRING_TYPE,
                             loc.literal = s; };

    Literal(char c) { kind = ExprKind::LITERAL,
                      t = CHAR_TYPE,
                      loc.literal = std::string(&c, 1); };

    Literal(Token _loc, int i) : loc(_loc)
    {
        kind = ExprKind::LITERAL;
        t = INT_TYPE;
        loc.literal = std::to_string(i);
    };

    Literal(Token _loc, double d) : loc(_loc)
    {
        kind = ExprKind::LITERAL;
        t = DOUBLE_TYPE;
        loc.literal = std::to_string(d);
    };

    Literal(Token _loc, bool b) : loc(_loc)
    {
        kind = ExprKind::LITERAL;
        t = BOOL_TYPE;
        loc.literal = b ? "true" : "false";
    };

    Literal(Token _loc, std::string s) : loc(_loc)
    {
        kind = ExprKind::LITERAL;
        t = STRING_TYPE;
        loc.literal = s;
    };

    Literal(Token _loc, char c) : loc(_loc)
    {
        kind = ExprKind::LITERAL;
        t = CHAR_TYPE;
        loc.literal = std::string(&c, 1);
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class Unary : public Expr
{
public:
    Token op;
    SP<Expr> right;

    Unary(const Token &_op, const SP<Expr> &_right)
    {
        kind = ExprKind::UNARY;
        op = _op;
        right = _right;
    };

    Token Loc() override { return op; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class Binary : public Expr
{
public:
    SP<Expr> left;
    Token op;
    SP<Expr> right;

    Binary(const SP<Expr> &_left, const Token &_op, const SP<Expr> &_right)
    {
        kind = ExprKind::BINARY;
        left = _left;
        op = _op;
        right = _right;
    };

    Token Loc() override { return op; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class VarReference : public Expr
{
public:
    Token loc;
    std::string name;
    VarReference(const Token &_loc)
    {
        kind = ExprKind::VAR_REFERENCE;
        loc = _loc;
        name = loc.literal;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class Assign : public Expr
{
public:
    Token loc;
    SP<Expr> target;
    SP<Expr> val;

    Assign(const SP<Expr> &_target, const SP<Expr> &_val, const Token &_loc)
    {
        kind = ExprKind::ASSIGN;
        target = _target;
        val = _val;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class FunctionCall : public Expr
{
public:
    Token loc;
    std::string name;
    std::vector<TypeData> templates;
    std::vector<SP<Expr>> args;

    FunctionCall(const std::string &_name, const std::vector<TypeData> &_templates, const std::vector<SP<Expr>> &_args, const Token &_loc)
    {
        kind = ExprKind::FUNCTION_CALL;
        name = _name;
        templates = _templates;
        args = _args;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class ArrayIndex : public Expr
{
public:
    Token loc;
    SP<Expr> name;
    SP<Expr> index;

    ArrayIndex(const SP<Expr> &_name, const SP<Expr> &_index, const Token &_loc)
    {
        kind = ExprKind::ARRAY_INDEX;
        name = _name;
        index = _index;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class BracedInitialiser : public Expr
{
public:
    Token loc;
    size_t size;
    std::vector<SP<Expr>> init;

    BracedInitialiser(const size_t &_size, const std::vector<SP<Expr>> &_init, const Token &_loc)
    {
        kind = ExprKind::BRACED_INITIALISER;
        size = _size;
        init = _init;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class DynamicAllocArray : public Expr
{
public:
    Token loc;
    SP<Expr> size;

    DynamicAllocArray(const TypeData &_t, const SP<Expr> &_size, const Token &_loc)
    {
        t = _t;
        size = _size;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class FieldAccess : public Expr
{
public:
    Token loc;
    SP<Expr> accessor;
    SP<Expr> accessee;

    FieldAccess(const SP<Expr> &_accessor, const SP<Expr> &_accessee, const Token &_loc)
    {
        kind = ExprKind::FIELD_ACCESS;
        accessor = _accessor;
        accessee = _accessee;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

class TypeCast : public Expr
{
public:
    Token loc;
    SP<Expr> arg;

    TypeCast(const TypeData &_type, const SP<Expr> &_arg, const Token &_loc)
    {
        kind = ExprKind::TYPE_CAST;
        t = _type;
        arg = _arg;
        loc = _loc;
    };

    Token Loc() override { return loc; };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};

// Sequence of integers, essentially a
// generalisation of summation/product
// notation
class Sequence : public Expr
{
public:
    Token loc;
    SP<Expr> start;
    SP<Expr> step;
    SP<Expr> end;
    SP<VarReference> var;
    SP<Expr> term;
    TokenID op;

    Sequence(SP<Expr> &_start,
             SP<Expr> &_step,
             SP<Expr> &_end,
             SP<VarReference> &_var,
             SP<Expr> &_term,
             TokenID _op,
             Token &_loc) : loc(_loc),
                            start(_start),
                            step(_step),
                            end(_end),
                            var(_var),
                            term(_term),
                            op(_op)
    {
        kind = ExprKind::SEQUENCE;
    };

    Token Loc() override
    {
        return loc;
    };
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    z3::expr CreateZ3Expr(Verifier &v) override;
};