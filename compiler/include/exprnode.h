#pragma once
#include "common.h"
#include "token.h"
#include <cstring>
#include <memory>

template <class T>
using SP = std::shared_ptr<T>;

class ASTPrinter;
class Compiler;
class StaticAnalyser;
class PostCondition;

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
    virtual Token Loc() = 0;
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(ASTPrinter &p) = 0;
    // ensures that the node is a valid node or throws error
    virtual TypeData Analyse(StaticAnalyser &sa) = 0;
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

    Literal(int i)
    {
        kind = ExprKind::LITERAL;
        t = INT_TYPE;
        loc.literal = std::to_string(i);
    };

    Literal(double d)
    {
        kind = ExprKind::LITERAL;
        t = DOUBLE_TYPE;
        loc.literal = std::to_string(d);
    };

    Literal(bool b)
    {
        kind = ExprKind::LITERAL;
        t = BOOL_TYPE;
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

    // ~Literal() override = default;

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class Unary : public Expr
{
public:
    Token op;
    SP<Expr> right;

    Unary(Token, SP<Expr>);
    // ~Unary() override = default;

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class Binary : public Expr
{
public:
    SP<Expr> left;
    Token op;
    SP<Expr> right;

    Binary(SP<Expr>, Token, SP<Expr>);
    // ~Binary() override = default;

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
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
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class Assign : public Expr
{
public:
    Token loc;
    SP<Expr> target;
    SP<Expr> val;

    Assign(SP<Expr> _target, SP<Expr> _val, Token _loc);
    // ~Assign() override = default;

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
    // bool IsTruthy() override;
};

class FunctionCall : public Expr
{
public:
    Token loc;
    std::string name;
    std::vector<TypeData> templates;
    std::vector<SP<Expr>> args;

    FunctionCall(std::string _name, std::vector<TypeData> _templates, std::vector<SP<Expr>> _args, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
};

class ArrayIndex : public Expr
{
public:
    Token loc;
    SP<Expr> name;
    SP<Expr> index;

    ArrayIndex(SP<Expr> _name, SP<Expr> _index, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
};

class BracedInitialiser : public Expr
{
public:
    TypeData t;
    Token loc;
    size_t size;
    std::vector<SP<Expr>> init;

    BracedInitialiser(size_t _size, std::vector<SP<Expr>> _init, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
};

class DynamicAllocArray : public Expr
{
public:
    TypeData t;
    Token loc;
    SP<Expr> size;

    DynamicAllocArray(TypeData _t, SP<Expr> _size, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
};

class FieldAccess : public Expr
{
public:
    Token loc;
    SP<Expr> accessor;
    SP<Expr> accessee;

    FieldAccess(SP<Expr> _accessor, SP<Expr> _accessee, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
};

class TypeCast : public Expr
{
public:
    Token loc;
    TypeData type;
    SP<Expr> arg;
    bool isDownCast = false;

    TypeCast(TypeData _type, SP<Expr> _arg, Token _loc);

    Token Loc() override;
    void Print(ASTPrinter &p) override;
    TypeData Analyse(StaticAnalyser &sa) override;
    TypeData NodeCompile(Compiler &c) override;
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
    TypeData NodeCompile(Compiler &c) override;
};