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

void PostCondition::AddReturnValue(const SP<Expr> &ret)
{
    for (auto &c : conditions)
        post.back().push_back(c);

    post.back().push_back(ret);
    post.push_back(std::vector<SP<Expr>>());
}

void PostCondition::AddCondition(const SP<Expr> &c)
{
    conditions.push_back(c);
}

void PostCondition::RemoveLastCondition()
{
    conditions.pop_back();
}

void PostCondition::PostConditionError(Token loc, std::string err)
{
    Error e = Error("[POST CONDITION ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

std::vector<std::vector<SP<Expr>>> PostCondition::Generate(SP<Stmt> &function)
{
    function->GeneratePost(*this);
    return post;
}

//------------------STATEMENTS---------------------//

void PostCondition::GenerateFromExprStmt(ExprStmt *)
{
}

void PostCondition::GenerateFromDeclaredVar(DeclaredVar *)
{
}

void PostCondition::GenerateFromBlock(Block *b)
{
    for (auto &stmt : b->stmts)
        stmt->GeneratePost(*this);
}

void PostCondition::GenerateFromIfStmt(IfStmt *i)
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

void PostCondition::GenerateFromWhileStmt(WhileStmt *ws)
{
    PostConditionError(ws->Loc(), "Cannot generate a post condition from a while loop");
}

void PostCondition::GenerateFromFuncDecl(FuncDecl *fd)
{
    if (fd->ret == VOID_TYPE)
        PostConditionError(fd->Loc(), "Cannot generate post condition errror for a void function");

    for (auto &stmt : fd->body)
        stmt->GeneratePost(*this);
}

void PostCondition::GenerateFromReturn(Return *r)
{
    if (r->retVal == nullptr)
        PostConditionError(r->Loc(), "Cannot generate post condition for a void-return");

    AddReturnValue(MAKE_RETURN(r->retVal));
}

void PostCondition::GenerateFromStructDecl(StructDecl *)
{
}

void PostCondition::GenerateFromImportStmt(ImportStmt *)
{
}

void PostCondition::GenerateFromBreak(Break *)
{
}

void PostCondition::GenerateFromThrow(Throw *t)
{
    PostConditionError(t->Loc(), "Cannot generate post condition for a function which throws an object");
}

void PostCondition::GenerateFromTryCatch(TryCatch *tc)
{
    PostConditionError(tc->Loc(), "Cannot generate post condition for a function which uses try-catch");
}

//------------------STATEMENTS---------------------//

void ExprStmt::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromExprStmt(this);
}

void DeclaredVar::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromDeclaredVar(this);
}

void Block::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromBlock(this);
}

void IfStmt::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromIfStmt(this);
}

void WhileStmt::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromWhileStmt(this);
}

void FuncDecl::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromFuncDecl(this);
}

void Return::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromReturn(this);
}

void StructDecl::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromStructDecl(this);
}

void ImportStmt::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromImportStmt(this);
}

void Break::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromBreak(this);
}

void Throw::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromThrow(this);
}

void TryCatch::GeneratePost(PostCondition &pc)
{
    pc.GenerateFromTryCatch(this);
}