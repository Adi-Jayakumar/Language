#pragma once
#include "stmtnode.h"
#include "perror.h"
#include <unordered_map>
#include <unordered_set>

struct TypeInfo
{
    uint8_t left;
    TokenID t;
    uint8_t right;
};

bool operator==(const TypeInfo &l, const TypeInfo &r);

struct TypeInfoHasher
{
    size_t operator()(const TypeInfo &t) const
    {
        size_t l = static_cast<size_t>(t.left);
        size_t r = static_cast<size_t>(t.right);
        size_t tok = static_cast<size_t>(t.t);
        return (l << 16) | (r << 8) | (tok);
    }
};

namespace TypeChecker
{
    void TypeError(Token loc, std::string err);

    // expression typechecking
    TypeID TypeOfLiteral(Literal *l);
    TypeID TypeOfUnary(Unary *u);
    TypeID TypeOfBinary(Binary *b);
    TypeID TypeOfAssign(Assign *a);
    TypeID TypeOfVarReference(VarReference *vr);

    // statment typechecking
    TypeID TypeOfExprStmt(ExprStmt *es);
    TypeID TypeOfDeclaredVar(DeclaredVar *v);
} // namespace TypeChecker

static const std::unordered_map<TypeInfo, TypeID, TypeInfoHasher>
    OperatorMap{
        {{1, TokenID::PLUS, 1}, 1},
        {{1, TokenID::PLUS, 2}, 2},
        {{2, TokenID::PLUS, 1}, 2},
        {{2, TokenID::PLUS, 2}, 2},
        {{1, TokenID::MINUS, 1}, 1},
        {{1, TokenID::MINUS, 2}, 2},
        {{2, TokenID::MINUS, 1}, 2},
        {{2, TokenID::MINUS, 2}, 2},
        {{1, TokenID::STAR, 1}, 1},
        {{1, TokenID::STAR, 2}, 2},
        {{2, TokenID::STAR, 1}, 2},
        {{2, TokenID::STAR, 2}, 2},
        {{1, TokenID::SLASH, 1}, 1},
        {{1, TokenID::SLASH, 2}, 2},
        {{2, TokenID::SLASH, 1}, 2},
        {{2, TokenID::SLASH, 2}, 2},
        {{1, TokenID::GT, 1}, 3},
        {{1, TokenID::GT, 2}, 3},
        {{2, TokenID::GT, 1}, 3},
        {{2, TokenID::GT, 2}, 3},
        {{1, TokenID::LT, 1}, 3},
        {{1, TokenID::LT, 2}, 3},
        {{2, TokenID::LT, 1}, 3},
        {{2, TokenID::LT, 2}, 3},
        {{1, TokenID::GEQ, 1}, 3},
        {{1, TokenID::GEQ, 2}, 3},
        {{2, TokenID::GEQ, 1}, 3},
        {{2, TokenID::GEQ, 2}, 3},
        {{1, TokenID::LEQ, 1}, 3},
        {{1, TokenID::LEQ, 2}, 3},
        {{2, TokenID::LEQ, 1}, 3},
        {{2, TokenID::LEQ, 2}, 3},
        {{1, TokenID::EQ_EQ, 1}, 3},
        {{1, TokenID::EQ_EQ, 2}, 3},
        {{2, TokenID::EQ_EQ, 1}, 3},
        {{2, TokenID::EQ_EQ, 2}, 3},
        {{1, TokenID::BANG_EQ, 1}, 3},
        {{1, TokenID::BANG_EQ, 2}, 3},
        {{2, TokenID::BANG_EQ, 1}, 3},
        {{2, TokenID::BANG_EQ, 2}, 3},
        {{3, TokenID::BANG_EQ, 3}, 3},
        {{0, TokenID::BANG, 3}, 3},
    };