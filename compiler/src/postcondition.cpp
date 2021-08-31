#include "postcondition.h"

inline SP<Expr> NEGATE(const SP<Expr> &cond)
{
    Token loc = Token(TokenID::BANG, "!", cond->Loc().line);
    return std::make_shared<Unary>(loc, cond);
}

inline SP<Expr> MAKE_RETURN(const SP<Expr> &val)
{
    Token resLoc = Token(TokenID::IDEN, "result", val->Loc().line);
    SP<Expr> result = std::make_shared<VarReference>(resLoc);

    Token eqeqLoc = Token(TokenID::EQ_EQ, "==", val->Loc().line);
    return std::make_shared<Binary>(result, eqeqLoc, val);
}

void PostConditionGenerator::AddReturnValue(const SP<Expr> &ret)
{
    for (auto &c : conditions)
        post.back().push_back(c);

    post.back().push_back(ret);
    post.push_back(std::vector<SP<Expr>>());
}

void PostConditionGenerator::AddCondition(const SP<Expr> &c)
{
    conditions.push_back(c);
}

void PostConditionGenerator::RemoveLastCondition()
{
    conditions.pop_back();
}

void PostConditionGenerator::PostConditionError(Token loc, std::string err)
{
    Error e = Error("[POST CONDITION ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

std::vector<std::vector<SP<Expr>>> PostConditionGenerator::Generate(SP<Stmt> &function)
{
    function->GeneratePost(*this);
    post.pop_back();
    return post;
}

//------------------STATEMENTS---------------------//

void PostConditionGenerator::GenerateFromExprStmt(ExprStmt *)
{
}

void PostConditionGenerator::GenerateFromDeclaredVar(DeclaredVar *)
{
}

void PostConditionGenerator::GenerateFromBlock(Block *b)
{
    for (auto &stmt : b->stmts)
        stmt->GeneratePost(*this);
}

void PostConditionGenerator::GenerateFromIfStmt(IfStmt *i)
{
    AddCondition(i->cond);
    i->thenBranch->GeneratePost(*this);
    RemoveLastCondition();

    if (i->elseBranch != nullptr)
    {
        AddCondition(NEGATE(i->cond));
        i->elseBranch->GeneratePost(*this);
        RemoveLastCondition();
    }
}

void PostConditionGenerator::GenerateFromWhileStmt(WhileStmt *ws)
{
    PostConditionError(ws->Loc(), "Cannot generate a post condition from a while loop");
}

void PostConditionGenerator::GenerateFromFuncDecl(FuncDecl *fd)
{
    if (fd->ret == VOID_TYPE)
        PostConditionError(fd->Loc(), "Cannot generate post condition errror for a void function");

    for (auto &stmt : fd->body)
        stmt->GeneratePost(*this);
}

void PostConditionGenerator::GenerateFromReturn(Return *r)
{
    if (r->retVal == nullptr)
        PostConditionError(r->Loc(), "Cannot generate post condition for a void-return");

    AddReturnValue(MAKE_RETURN(r->retVal));
}

void PostConditionGenerator::GenerateFromStructDecl(StructDecl *)
{
}

void PostConditionGenerator::GenerateFromImportStmt(ImportStmt *)
{
}

void PostConditionGenerator::GenerateFromBreak(Break *)
{
}

void PostConditionGenerator::GenerateFromThrow(Throw *t)
{
    PostConditionError(t->Loc(), "Cannot generate post condition for a function which throws an object");
}

void PostConditionGenerator::GenerateFromTryCatch(TryCatch *tc)
{
    PostConditionError(tc->Loc(), "Cannot generate post condition for a function which uses try-catch");
}

//------------------STATEMENTS---------------------//

void ExprStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromExprStmt(this);
}

void DeclaredVar::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromDeclaredVar(this);
}

void Block::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromBlock(this);
}

void IfStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromIfStmt(this);
}

void WhileStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromWhileStmt(this);
}

void FuncDecl::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromFuncDecl(this);
}

void Return::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromReturn(this);
}

void StructDecl::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromStructDecl(this);
}

void ImportStmt::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromImportStmt(this);
}

void Break::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromBreak(this);
}

void Throw::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromThrow(this);
}

void TryCatch::GeneratePost(PostConditionGenerator &pc)
{
    pc.GenerateFromTryCatch(this);
}