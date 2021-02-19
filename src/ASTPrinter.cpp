#include "ASTPrinter.h"

std::ostream &operator<<(std::ostream &out, Expr *e)
{
    if (e == nullptr)
        out << "null";
    else
        e->Print(out);
    return out;
}

std::ostream &operator<<(std::ostream &out, Stmt *s)
{
    if (s == nullptr)
        out << "null";
    else
        s->Print(out);
    return out;
}

//-----------------EXPRESSIONS---------------------//

void ASTPrinter::PrintLiteral(Literal *l, std::ostream &out)
{
    if (l == nullptr)
        out << "null";
    else
    {
        out << "<" << TypeStringMap.at(l->typeID) << "> ";
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
}

void ASTPrinter::PrintUnary(Unary *u, std::ostream &out)
{
    if (u == nullptr)
        out << "null";
    else
    {
        out << "<" << TypeStringMap.at(u->typeID) << "> ";
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
}

void ASTPrinter::PrintBinary(Binary *b, std::ostream &out)
{
    if (b == nullptr)
        out << "null";
    else
    {
        out << "<" << TypeStringMap.at(b->typeID) << "> ";
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
}

void ASTPrinter::PrintAssign(Assign *a, std::ostream &out)
{
    if (a == nullptr)
        out << "null";
    else
    {
        a->var->Print(out);
        if (a->val == nullptr)
            out << " = null";
        else
        {
            out << " = ";
            a->val->Print(out);
        }
    }
}

void ASTPrinter::PrintVarReference(VarReference *vr, std::ostream &out)
{
    if (vr == nullptr)
        out << "null";
    else
    {
        if (vr->isArray)
            out << "Array";
        out << "<" << TypeStringMap.at(vr->typeID) << "> ";
        out << vr->name;
    }
}

void ASTPrinter::PrintFunctionCall(FunctionCall *fc, std::ostream &out)
{
    out << "<" << TypeStringMap.at(fc->typeID) << "> ";
    out << fc->name << "(";
    for (auto &e : fc->args)
    {
        e->Print(out);
        out << ", ";
    }
    out << ")";
}

void ASTPrinter::PrintArrayIndex(ArrayIndex *ai, std::ostream &out)
{
    out << "<" << TypeStringMap[ai->typeID] << "> " << ai->name << "[";
    ai->index->Print(out);
    out << "]";
}

//------------------STATEMENTS---------------------//

void ASTPrinter::PrintExprStmt(ExprStmt *es, std::ostream &out)
{
    if (es == nullptr)
        out << "null";
    else
    {
        es->exp->Print(out);
        out << ";" << std::endl;
    }
}

void ASTPrinter::PrintDeclaredVar(DeclaredVar *v, std::ostream &out)
{
    if (v == nullptr)
        out << "null";
    else
        out << TypeStringMap.at(v->tId) << " " << v->name << " = " << v->value.get();
    out << ";" << std::endl;
}

void ASTPrinter::PrintArrayDecl(ArrayDecl *ad, std::ostream &out)
{
    if (ad == nullptr)
        out << "null";
    else
    {
        out << "Array<" << TypeStringMap[ad->elemType] << "> " << ad->name << " = ";
        if (ad->init.size() == 0)
            out << "null";
        else
        {
            out << "{";
            for (auto &e : ad->init)
            {
                e->Print(out);
                out << ", ";
            }
            out << "}";
        }
        out << ";";
    }
    out << std::endl;
}

void ASTPrinter::PrintBlock(Block *b, std::ostream &out)
{
    if (b == nullptr)
        out << "null";
    else
    {

        out << "DEPTH: " << +b->depth << std::endl;
        for (uint8_t i = 1; i < b->depth; i++)
            out << "\t";
        out << "{\n";

        for (std::shared_ptr<Stmt> &s : b->stmts)
        {
            for (uint8_t i = 1; i < b->depth; i++)
                out << "\t";
            out << "\t" << s.get() << std::endl;
        }

        for (uint8_t i = 1; i < b->depth; i++)
            out << "\t";
        out << "}\n";
    }
}

void ASTPrinter::PrintIfStmt(IfStmt *i, std::ostream &out)
{
    out << "if (";
    i->cond->Print(out);
    out << ")\n";
    i->thenBranch->Print(out);
    if (i->elseBranch != nullptr)
    {
        out << "\nelse";
        i->elseBranch->Print(out);
    }
}

void ASTPrinter::PrintWhileStmt(WhileStmt *ws, std::ostream &out)
{
    out << "while(";
    ws->cond->Print(out);
    out << ")\n";
    ws->body->Print(out);
}

void ASTPrinter::PrintFuncDecl(FuncDecl *fd, std::ostream &out)
{
    out << TypeStringMap.at(fd->ret) << " ";
    out << fd->name << "(";
    for (const Token &t : fd->params)
    {
        out << t.literal;
        if (t.type == TokenID::TYPENAME)
            out << " ";
        else
            out << ", ";
    }
    out << ")" << std::endl;
    out << "{" << std::endl;
    for (auto &s : fd->body)
    {
        out << "\t";
        s->Print(out);
    }
    out << std::endl
        << "}";
}

void ASTPrinter::PrintReturn(Return *r, std::ostream &out)
{
    out << "return ";
    if (r->retVal != nullptr)
        r->retVal->Print(out);
    out << ";";
    return;
}

//-----------------EXPRESSIONS---------------------//

void Literal::Print(std::ostream &out)
{
    ASTPrinter::PrintLiteral(this, out);
}

void Unary::Print(std::ostream &out)
{
    ASTPrinter::PrintUnary(this, out);
}

void Binary::Print(std::ostream &out)
{
    ASTPrinter::PrintBinary(this, out);
}

void Assign::Print(std::ostream &out)
{
    ASTPrinter::PrintAssign(this, out);
}

void VarReference::Print(std::ostream &out)
{
    ASTPrinter::PrintVarReference(this, out);
}

void FunctionCall::Print(std::ostream &out)
{
    ASTPrinter::PrintFunctionCall(this, out);
}

void ArrayIndex::Print(std::ostream &out)
{
    ASTPrinter::PrintArrayIndex(this, out);
}
//------------------STATEMENTS---------------------//

void ExprStmt::Print(std::ostream &out)
{
    ASTPrinter::PrintExprStmt(this, out);
}

void DeclaredVar::Print(std::ostream &out)
{
    ASTPrinter::PrintDeclaredVar(this, out);
}

void ArrayDecl::Print(std::ostream &out)
{
    ASTPrinter::PrintArrayDecl(this, out);
}

void Block::Print(std::ostream &out)
{
    ASTPrinter::PrintBlock(this, out);
}

void IfStmt::Print(std::ostream &out)
{
    ASTPrinter::PrintIfStmt(this, out);
}

void WhileStmt::Print(std::ostream &out)
{
    ASTPrinter::PrintWhileStmt(this, out);
}

void FuncDecl::Print(std::ostream &out)
{
    ASTPrinter::PrintFuncDecl(this, out);
}

void Return::Print(std::ostream &out)
{
    ASTPrinter::PrintReturn(this, out);
}