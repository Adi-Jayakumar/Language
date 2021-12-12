#include "ASTPrinter.h"

ASTPrinter &operator<<(ASTPrinter &ast, std::string str)
{
    ast.out << str;
    return ast;
}

//-----------------EXPRESSIONS---------------------//

void ASTPrinter::PrintLiteral(Literal *l)
{
    if (print_types)
    {
        symbols.PrintType(out, l->t);
        out << " ";
    }

    if (l->t.type == 4)
        out << "\"" << l->loc.literal << "\"";
    else if (l->t.type == 5)
        out << "\'" << l->loc.literal << "\'";
    else
        out << l->loc.literal;
}

void ASTPrinter::PrintUnary(Unary *u)
{
    if (u->op.type == TokenID::MINUS)
        out << "-";
    else if (u->op.type == TokenID::BANG)
        out << "!";
    else
        out << u->op;
    out << "(";
    u->right->Print(*this);
    out << ") ";
}

void ASTPrinter::PrintBinary(Binary *b)
{
    out << "(";
    b->left->Print(*this);

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
    else if (b->op.type == TokenID::AND_AND)
        out << " && ";
    else if (b->op.type == TokenID::OR_OR)
        out << " || ";
    else
        out << b->op;

    b->right->Print(*this);

    out << ")";
}

void ASTPrinter::PrintAssign(Assign *a)
{
    a->target->Print(*this);
    out << " = ";
    a->val->Print(*this);
}

void ASTPrinter::PrintVarReference(VarReference *vr)
{
    out << vr->name;
}

void ASTPrinter::PrintFunctionCall(FunctionCall *fc)
{
    out << fc->name;

    if (fc->templates.size() != 0)
    {
        out << "<";
        for (auto &t : fc->templates)
        {
            symbols.PrintType(out, t);
            out << ", ";
        }
        out << ">";
    }

    out << "(";
    for (size_t i = 0; i < fc->args.size() - 1; i++)
    {
        fc->args[i]->Print(*this);
        out << ", ";
    }
    fc->args[fc->args.size() - 1]->Print(*this);
    out << ")";
}

void ASTPrinter::PrintArrayIndex(ArrayIndex *ai)
{
    ai->name->Print(*this);
    out << "[";
    ai->index->Print(*this);
    out << "]";
}

void ASTPrinter::PrintBracedInitialiser(BracedInitialiser *ia)
{
    symbols.PrintType(out, ia->t);
    out << "{";
    for (size_t i = 0; i < ia->init.size(); i++)
    {
        ia->init[i]->Print(*this);
        if (i != ia->init.size() - 1)
            out << ", ";
    }
    out << "}";
}

void ASTPrinter::PrintDynamicAllocArray(DynamicAllocArray *da)
{
    symbols.PrintType(out, da->t);
    out << "[";
    da->size->Print(*this);
    out << "]";
}

void ASTPrinter::PrintFieldAccess(FieldAccess *fa)
{
    fa->accessor->Print(*this);
    out << ".";
    fa->accessee->Print(*this);
}

void ASTPrinter::PrintTypeCast(TypeCast *gf)
{
    out << "Cast<";
    symbols.PrintType(out, gf->t);
    out << ">(" << gf->arg.get() << ")";
}

void ASTPrinter::PrintSequence(Sequence *s)
{
    out << "Sequence(";
    s->start->Print(*this);
    out << ", ";
    s->step->Print(*this);
    out << ", ";
    s->end->Print(*this);
    out << ", ";
    s->var->Print(*this);
    out << ", ";
    s->term->Print(*this);
    out << ", ";
    out << ToString(s->op);
    out << ")";
}

//------------------STATEMENTS---------------------//

void ASTPrinter::PrintExprStmt(ExprStmt *es)
{
    es->exp->Print(*this);
    out << ";";
    NewLine();
}

void ASTPrinter::PrintDeclaredVar(DeclaredVar *v)
{
    symbols.PrintType(out, v->t);
    out << " " << v->name;

    if (v->value != nullptr)
    {
        out << " = ";
        v->value->Print(*this);
    }

    out << ";";
    NewLine();
}

void ASTPrinter::PrintBlock(Block *b)
{
    out << "{";
    depth++;
    NewLine();

    for (size_t i = 0; i < b->stmts.size() - 1; i++)
        b->stmts[i]->Print(*this);

    b->stmts[b->stmts.size() - 1]->Print(*this);
    depth--;
    NewLine();
    out << "}";
    NewLine();
}

void ASTPrinter::PrintIfStmt(IfStmt *i)
{
    out << "if (";
    i->cond->Print(*this);
    out << ")";

    NewLine();

    i->then_branch->Print(*this);
    if (i->else_branch != nullptr)
    {
        NewLine();
        out << "else";
        NewLine();
        i->else_branch->Print(*this);
    }
}

void ASTPrinter::PrintWhileStmt(WhileStmt *ws)
{
    out << "while (";
    ws->cond->Print(*this);
    out << ")";

    NewLine();
    ws->body->Print(*this);
}

void ASTPrinter::PrintFuncDecl(FuncDecl *fd)
{
    symbols.PrintType(out, fd->ret);
    out << " " << fd->name << "(";

    for (size_t i = 0; i < fd->params.size(); i++)
    {
        symbols.PrintType(out, fd->params[i].first);
        out << " " << fd->params[i].second;
        if (i != fd->params.size() - 1)
            out << ", ";
    }

    out << ")";

    NewLine();

    if (fd->pre_conds.size() != 0)
    {
        out << "(|";
        for (auto exp : fd->pre_conds)
        {
            exp->Print(*this);
            out << ";";
        }
        out << "|)";
        NewLine();
    }

    depth++;
    out << "{";
    NewLine();

    if (fd->body.size() > 0)
    {
        for (size_t i = 0; i < fd->body.size() - 1; i++)
            fd->body[i]->Print(*this);

        fd->body[fd->body.size() - 1]->Print(*this);
    }

    depth--;
    NewLine();
    out << "}";
    NewLine();

    if (fd->post_cond != nullptr)
    {
        out << "(|";
        fd->post_cond->Print(*this);
        out << "|)";
    }

    NewLine();
    NewLine();
}

void ASTPrinter::PrintTemplateDecl(TemplateDecl *td)
{
    out << "template<";
    for (size_t i = 0; i < td->templates.size(); i++)
    {
        symbols.PrintType(out, td->templates[i].first);
        if (i != td->templates.size() - 1)
            out << ", ";
    }
    out << ">";
    NewLine();
    td->stmt->Print(*this);
}

void ASTPrinter::PrintReturn(Return *r)
{
    out << "return ";
    if (r->ret_val != nullptr)
        r->ret_val->Print(*this);

    out << ";";
    NewLine();
}

void ASTPrinter::PrintStructDecl(StructDecl *sd)
{

    out << "struct typename " << sd->name;
    NewLine();
    out << "{";

    depth++;
    NewLine();
    for (auto &d : sd->decls)
        d->Print(*this);
    depth--;
    NewLine();
    out << "}";
    NewLine();
}

void ASTPrinter::PrintImportStmt(ImportStmt *is)
{
    out << "import ";
    for (const auto &str : is->libraries)
        out << str << ", ";

    NewLine();
}

void ASTPrinter::PrintBreak(Break *)
{
    out << "break;";
}

void ASTPrinter::PrintThrow(Throw *t)
{
    out << "throw ";
    t->exp->Print(*this);
    out << ";";
    NewLine();
}

void ASTPrinter::PrintTryCatch(TryCatch *tc)
{
    out << "try";
    NewLine();

    tc->try_clause->Print(*this);
    NewLine();

    out << "catch (";
    symbols.PrintType(out, tc->catch_var.first);
    out << " " << tc->catch_var.second << ")";
    NewLine();

    tc->catch_clause->Print(*this);
}

//-----------------EXPRESSIONS---------------------//

void Literal::Print(ASTPrinter &p)
{
    p.PrintLiteral(this);
}

void Unary::Print(ASTPrinter &p)
{
    p.PrintUnary(this);
}

void Binary::Print(ASTPrinter &p)
{
    p.PrintBinary(this);
}

void Assign::Print(ASTPrinter &p)
{
    p.PrintAssign(this);
}

void VarReference::Print(ASTPrinter &p)
{
    p.PrintVarReference(this);
}

void FunctionCall::Print(ASTPrinter &p)
{
    p.PrintFunctionCall(this);
}

void ArrayIndex::Print(ASTPrinter &p)
{
    p.PrintArrayIndex(this);
}

void BracedInitialiser::Print(ASTPrinter &p)
{
    p.PrintBracedInitialiser(this);
}

void DynamicAllocArray::Print(ASTPrinter &p)
{
    p.PrintDynamicAllocArray(this);
}

void FieldAccess::Print(ASTPrinter &p)
{
    p.PrintFieldAccess(this);
}

void TypeCast::Print(ASTPrinter &p)
{
    p.PrintTypeCast(this);
}

void Sequence::Print(ASTPrinter &p)
{
    p.PrintSequence(this);
}

//------------------STATEMENTS---------------------//

void ExprStmt::Print(ASTPrinter &p)
{
    p.PrintExprStmt(this);
}

void DeclaredVar::Print(ASTPrinter &p)
{
    p.PrintDeclaredVar(this);
}

void Block::Print(ASTPrinter &p)
{
    p.PrintBlock(this);
}

void IfStmt::Print(ASTPrinter &p)
{
    p.PrintIfStmt(this);
}

void WhileStmt::Print(ASTPrinter &p)
{
    p.PrintWhileStmt(this);
}

void FuncDecl::Print(ASTPrinter &p)
{
    p.PrintFuncDecl(this);
}

void TemplateDecl::Print(ASTPrinter &p)
{
    p.PrintTemplateDecl(this);
}

void Return::Print(ASTPrinter &p)
{
    p.PrintReturn(this);
}

void StructDecl::Print(ASTPrinter &p)
{
    p.PrintStructDecl(this);
}

void ImportStmt::Print(ASTPrinter &p)
{
    p.PrintImportStmt(this);
}

void Break::Print(ASTPrinter &p)
{
    p.PrintBreak(this);
}

void Throw::Print(ASTPrinter &p)
{
    p.PrintThrow(this);
}

void TryCatch::Print(ASTPrinter &p)
{
    p.PrintTryCatch(this);
}