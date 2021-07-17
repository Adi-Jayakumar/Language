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

//-----------------EXPRESSIONS---------------------//

void ASTPrinter::PrintLiteral(Literal *l, std::ostream &out)
{
    out << l->t;
    if (l->t.type == 4)
        out << " \"" << l->loc.literal << "\"";
    else if (l->t.type == 5)
        out << " \'" << l->loc.literal << "\'";
    else
        out << l->loc.literal;
}

void ASTPrinter::PrintUnary(Unary *u, std::ostream &out)
{
    out << u->t;
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

void ASTPrinter::PrintBinary(Binary *b, std::ostream &out)
{
    out << b->t;
    out << " (";
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

void ASTPrinter::PrintAssign(Assign *a, std::ostream &out)
{
    a->target->Print(out);
    if (a->val == nullptr)
        out << " = null";
    else
    {
        out << " = ";
        a->val->Print(out);
    }
}

void ASTPrinter::PrintVarReference(VarReference *vr, std::ostream &out)
{
    out << vr->t << " " << vr->name;
}

void ASTPrinter::PrintFunctionCall(FunctionCall *fc, std::ostream &out)
{
    out << fc->t << " " << fc->name << "(";
    for (auto &e : fc->args)
    {
        e->Print(out);
        out << ", ";
    }
    out << ")";
}

void ASTPrinter::PrintArrayIndex(ArrayIndex *ai, std::ostream &out)
{
    out << ai->t << " (";
    ai->name->Print(out);
    out << "[";
    ai->index->Print(out);
    out << "])";
}

void ASTPrinter::PrintBracedInitialiser(BracedInitialiser *ia, std::ostream &out)
{
    out << ia->t << " {";
    for (size_t i = 0; i < ia->init.size(); i++)
    {
        ia->init[i]->Print(out);
        if (i != ia->init.size() - 1)
            out << ", ";
    }
    out << "}";
}

void ASTPrinter::PrintDynamicAllocArray(DynamicAllocArray *da, std::ostream &out)
{
    out << da->t << "[";
    da->size->Print(out);
    out << "]";
}

void ASTPrinter::PrintFieldAccess(FieldAccess *fa, std::ostream &out)
{
    out << fa->t << " (";
    fa->accessor->Print(out);
    out << ".";
    fa->accessee->Print(out);
    out << ")";
}

void ASTPrinter::PrintTypeCast(TypeCast *gf, std::ostream &out)
{
    out << gf->t << " Cast";
    gf->type.isArray ? out << "<" << gf->type << ">" : out << gf->type;
    out << "(" << gf->arg.get() << ")";
}

//------------------STATEMENTS---------------------//

void ASTPrinter::PrintExprStmt(ExprStmt *es, std::ostream &out)
{

    es->exp->Print(out);
    out << ";" << std::endl;
}

void ASTPrinter::PrintDeclaredVar(DeclaredVar *v, std::ostream &out)
{
    out << v->t << " " << v->name;
    if (v->value != nullptr)
        out << " = " << v->value.get();
    out << ";" << std::endl;
}

void ASTPrinter::PrintBlock(Block *b, std::ostream &out)
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
    out << fd->ret << " " << fd->name << "(";

    for (size_t i = 0; i < fd->argtypes.size(); i++)
    {
        out << fd->argtypes[i] << " " << fd->paramIdentifiers[i];
        if (i != fd->argtypes.size() - 1)
            out << ", ";
    }

    out << ")" << std::endl;

    if (fd->preConds.size() != 0)
    {
        out << "(|";
        for (auto exp : fd->preConds)
        {
            exp->Print(out);
            out << ";";
        }
        out << "|)" << std::endl;
    }

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
    out << ";" << std::endl;

    if (r->postConds.size() != 0)
    {
        out << "(|";
        for (auto exp : r->postConds)
        {
            exp->Print(out);
            out << ";";
        }
        out << "|)" << std::endl;
    }

    return;
}

void ASTPrinter::PrintStructDecl(StructDecl *sd, std::ostream &out)
{
    sd->parent.type ? out << "struct " << sd->name << " : " << ToString(sd->parent) << "\n{\n" : out << "struct " << sd->name << "\n{\n";
    for (auto &d : sd->decls)
        out << "\t" << d;
    out << "}";
}

void ASTPrinter::PrintImportStmt(ImportStmt *is, std::ostream &out)
{
    out << "import ";
    for (const auto &str : is->libraries)
        out << str << ", ";
    out << std::endl;
}

void ASTPrinter::PrintBreak(Break *, std::ostream &out)
{
    out << "break;";
}

void ASTPrinter::PrintThrow(Throw *t, std::ostream &out)
{
    out << "throw ";
    t->exp->Print(out);
    out << std::endl;
}

void ASTPrinter::PrintTryCatch(TryCatch *tc, std::ostream &out)
{
    out << "try" << std::endl;
    tc->tryClause->Print(out);
    out << std::endl;
    out << "catch(" << tc->catchVar.first << " " << tc->catchVar.second << ")" << std::endl;
    tc->catchClause->Print(out);
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

void BracedInitialiser::Print(std::ostream &out)
{
    ASTPrinter::PrintBracedInitialiser(this, out);
}

void DynamicAllocArray::Print(std::ostream &out)
{
    ASTPrinter::PrintDynamicAllocArray(this, out);
}

void FieldAccess::Print(std::ostream &out)
{
    ASTPrinter::PrintFieldAccess(this, out);
}

void TypeCast::Print(std::ostream &out)
{
    ASTPrinter::PrintTypeCast(this, out);
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

void StructDecl::Print(std::ostream &out)
{
    ASTPrinter::PrintStructDecl(this, out);
}

void ImportStmt::Print(std::ostream &out)
{
    ASTPrinter::PrintImportStmt(this, out);
}

void Break::Print(std::ostream &out)
{
    ASTPrinter::PrintBreak(this, out);
}

void Throw::Print(std::ostream &out)
{
    ASTPrinter::PrintThrow(this, out);
}

void TryCatch::Print(std::ostream &out)
{
    ASTPrinter::PrintTryCatch(this, out);
}