#pragma once
#include "exprnode.h"

namespace ASTPrinter
{
    void PrintLiteral(Literal *l, std::ostream &out);
    void PrintUnary(Unary *u, std::ostream &out);
    void PrintGrouping(Grouping *g, std::ostream &out);
    void PrintBinary(Binary *b, std::ostream &out);
};