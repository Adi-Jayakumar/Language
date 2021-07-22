#pragma once
#include "stmtnode.h"
#include <memory>

std::shared_ptr<Literal> BINARY_PLUS(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_MINUS(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_STAR(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_SLASH(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_GT(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_LT(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_GEQ(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_LEQ(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_EQ_EQ(Literal *left, Literal *right);
std::shared_ptr<Literal> BINARY_BANG_EQ(Literal *left, Literal *right);

std::shared_ptr<Literal> UNARY_MINUS(Literal *right);
std::shared_ptr<Literal> UNARY_BANG(Literal *right);