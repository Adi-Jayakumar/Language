#include "ASTPrinter.h"

std::ostream &operator<<(std::ostream &out, Expr *e)
{
    e->Print(out);
    return out;
}

void ASTPrinter::PrintLiteral(Literal *l, std::ostream &out)
{
    out << std::to_string(l->val);
}

void ASTPrinter::PrintUnary(Unary *u, std::ostream &out)
{
    out << u->op << "(";
    u->right->Print(out);
    out << ")";
}

void ASTPrinter::PrintGrouping(Grouping *g, std::ostream &out)
{
    g->exp->Print(out);
}

void ASTPrinter::PrintBinary(Binary *b, std::ostream &out)
{
    out << "(";
    b->left->Print(out);
    
    if(b->op.type == TokenID::PLUS)
        out << " + ";
    else if(b->op.type == TokenID::MINUS)
        out << " - ";
    else if(b->op.type == TokenID::STAR)
        out << " * ";
    else if(b->op.type == TokenID::SLASH)
        out << " / ";
    else
        out << b->op;
    
    b->right->Print(out);
    
    out << ")";
}

void Literal::Print(std::ostream &out)
{
    ASTPrinter::PrintLiteral(this, out);
}

void Unary::Print(std::ostream &out)
{
    ASTPrinter::PrintUnary(this, out);
}

void Grouping::Print(std::ostream &out)
{
    ASTPrinter::PrintGrouping(this, out);
}

void Binary::Print(std::ostream &out)
{
    ASTPrinter::PrintBinary(this, out);
}
