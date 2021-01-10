#pragma once
#include "stmtnode.h"

namespace ASTPrinter
{
    // expression printing
    void PrintLiteral(Literal *l, std::ostream &out);
    void PrintUnary(Unary *u, std::ostream &out);
    void PrintBinary(Binary *b, std::ostream &out);
    
    // statment printing
    void PrintExprStmt(ExprStmt *es, std::ostream &out);
};