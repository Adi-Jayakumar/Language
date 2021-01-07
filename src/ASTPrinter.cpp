#include "ASTPrinter.h"

std::ostream &operator<<(std::ostream &out, Expr *e)
{
    e->Print(out);
    return out;
}

std::ostream &operator<<(std::ostream &out, Stmt *s)
{
    s->Print(out);
    return out;
}

void ASTPrinter::PrintLiteral(Literal *l, std::ostream &out)
{
    switch (l->typeID)
    {
    case 1:
    {
        out << l->as.i;
        break;
    }
    case 2:
    {
        out << l->as.d;
        break;
    }
    case 3:
    {
        if (l->as.b)
            out << "true";
        else
            out << "false";
        break;
    }
    }
}

void ASTPrinter::PrintUnary(Unary *u, std::ostream &out)
{
    if (u->op.type == TokenID::MINUS)
        out << " -";
    else if (u->op.type == TokenID::BANG)
        out << " !";
    else
        out << u->op;
    out << "(";
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

    if (b->op.type == TokenID::PLUS)
        out << " + ";
    else if (b->op.type == TokenID::MINUS)
        out << " - ";
    else if (b->op.type == TokenID::STAR)
        out << " * ";
    else if (b->op.type == TokenID::SLASH)
        out << " / ";
    else if (b->op.type == TokenID::GT)
        out << " > ";
    else if (b->op.type == TokenID::LT)
        out << " < ";
    else if (b->op.type == TokenID::GEQ)
        out << " >= ";
    else if (b->op.type == TokenID::LEQ)
        out << " <= ";
    else if (b->op.type == TokenID::EQ_EQ)
        out << " == ";
    else if (b->op.type == TokenID::BANG_EQ)
        out << " != ";
    else
        out << b->op;

    b->right->Print(out);

    out << ")";
}

void ASTPrinter::PrintExprStmt(ExprStmt *es, std::ostream &out)
{
    es->exp->Print(out);
    out << ";";
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

void ExprStmt::Print(std::ostream &out)
{
    ASTPrinter::PrintExprStmt(this, out);
}