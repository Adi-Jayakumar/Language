#include "ASTPrinter.h"

//-----------------EXPRESSIONS---------------------//

void ASTPrinter::PrintLiteral(Literal *l)
{
    if (printTypes)
        out << l->GetType();

    if (l->t.type == 4)
        out << " \"" << l->loc.literal << "\"";
    else if (l->t.type == 5)
        out << " \'" << l->loc.literal << "\'";
    else
        out << l->loc.literal;
}

void ASTPrinter::PrintUnary(Unary *u)
{

    if (printTypes)
        out << u->GetType();

    if (u->op.type == TokenID::MINUS)
        out << " -";
    else if (u->op.type == TokenID::BANG)
        out << " !";
    else
        out << u->op;
    out << "(";
    u->right->Print(*this);
    out << ") ";
}

void ASTPrinter::PrintBinary(Binary *b)
{
    if (printTypes)
        out << b->GetType();

    out << " (";
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
    else
        out << b->op;

    b->right->Print(*this);

    out << ") ";
}

void ASTPrinter::PrintAssign(Assign *a)
{
    a->target->Print(*this);
    out << " = ";
    a->val->Print(*this);
}

void ASTPrinter::PrintVarReference(VarReference *vr)
{
    if (printTypes)
        out << vr->t << " ";

    out << vr->name;
}

void ASTPrinter::PrintFunctionCall(FunctionCall *fc)
{
    if (printTypes)
        out << fc->t << " ";

    out << fc->name << "(";
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
    if (printTypes)
        out << ai->t << " (";

    ai->name->Print(*this);
    out << "[";
    ai->index->Print(*this);
    out << "]";

    if (printTypes)
        out << ")";
}

void ASTPrinter::PrintBracedInitialiser(BracedInitialiser *ia)
{
    out << ia->t << " {";
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
    out << da->t << "[";
    da->size->Print(*this);
    out << "]";
}

void ASTPrinter::PrintFieldAccess(FieldAccess *fa)
{
    if (printTypes)
        out << fa->t << " (";

    fa->accessor->Print(*this);
    out << ".";
    fa->accessee->Print(*this);

    if (printTypes)
        out << ")";
}

void ASTPrinter::PrintTypeCast(TypeCast *gf)
{
    if (printTypes)
        out << gf->t << " Cast";
    else
        out << "Cast";

    gf->type.isArray ? out << "<" << gf->type << ">" : out << gf->type;
    out << "(" << gf->arg.get() << ")";
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
    out << v->t << " " << v->name;

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
}

void ASTPrinter::PrintIfStmt(IfStmt *i)
{
    out << "if (";
    i->cond->Print(*this);
    out << ")";

    NewLine();

    i->thenBranch->Print(*this);
    if (i->elseBranch != nullptr)
    {
        NewLine();
        out << "else";
        NewLine();
        i->elseBranch->Print(*this);
    }
}

void ASTPrinter::PrintWhileStmt(WhileStmt *ws)
{
    if (ws->body == nullptr)
        return;

    out << "while (";
    ws->cond->Print(*this);
    out << ")";

    NewLine();
    ws->body->Print(*this);
}

void ASTPrinter::PrintFuncDecl(FuncDecl *fd)
{
    out << fd->ret << " " << fd->name << "(";

    for (size_t i = 0; i < fd->argtypes.size(); i++)
    {
        out << fd->argtypes[i] << " " << fd->paramIdentifiers[i];
        if (i != fd->argtypes.size() - 1)
            out << ", ";
    }

    out << ")";

    NewLine();

    if (fd->preConds.size() != 0)
    {
        out << "(|";
        for (auto exp : fd->preConds)
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

    for (size_t i = 0; i < fd->body.size() - 1; i++)
        fd->body[i]->Print(*this);

    fd->body[fd->body.size() - 1]->Print(*this);

    depth--;
    NewLine();
    out << "}";
    NewLine();
    NewLine();
}

void ASTPrinter::PrintReturn(Return *r)
{
    out << "return ";
    if (r->retVal != nullptr)
        r->retVal->Print(*this);
    out << ";";

    NewLine();

    if (r->postConds.size() != 0)
    {
        out << "(|";
        for (auto exp : r->postConds)
        {
            exp->Print(*this);
            out << ";";
        }
        out << "|)";
        NewLine();
    }

    return;
}

void ASTPrinter::PrintStructDecl(StructDecl *sd)
{

    out << sd->name;
    NewLine();
    out << "{";
    NewLine();

    depth++;
    for (auto &d : sd->decls)
        d->Print(*this);
    depth--;
    out << "}";
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

    tc->tryClause->Print(*this);
    NewLine();

    out << "catch(" << tc->catchVar.first << " " << tc->catchVar.second << ")";
    NewLine();

    tc->catchClause->Print(*this);
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