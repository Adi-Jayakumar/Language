#include "returnchecker.h"

void ReturnChecker::ReturnError(Token loc, std::string err)
{
    Error e = Error("[RETURN ERROR] On line " + std::to_string(loc.line) + "\n" + err);
    e.Dump();
}

bool ReturnChecker::ReturnCheckBlock(Block *b, TypeData ret)
{
    for (auto &s : b->stmts)
    {
        if (s->DoesReturn(ret))
            return true;
    }
    return false;
}

bool ReturnChecker::ReturnCheckIfStmt(IfStmt *is, TypeData ret)
{

    bool ans = false;

    Literal *lCond = dynamic_cast<Literal *>(is->cond.get());
    TypeData b = {false, 3};

    if (lCond && lCond->GetType() == b && lCond->as.b)
        ans = is->thenBranch->DoesReturn(ret);

    if (ans)
        return true;

    if (is->elseBranch == nullptr)
    {
        return false;
    }
    return is->thenBranch->DoesReturn(ret) && is->elseBranch->DoesReturn(ret);
}

bool ReturnChecker::ReturnCheckWhileStmt(WhileStmt *ws, TypeData ret)
{
    Literal *lCond = dynamic_cast<Literal *>(ws->cond.get());
    TypeData b = {false, 3};

    if (lCond && lCond->GetType() == b && lCond->as.b)
        return ws->body->DoesReturn(ret);
    return false;
}

bool ReturnChecker::ReturnCheckFuncDecl(FuncDecl *fd, TypeData ret)
{
    bool ans = false;
    for (auto &s : fd->body)
    {
        if (s->DoesReturn(ret))
        {
            ans = true;
            break;
        }
    }

    if (!ans)
        ReturnError(fd->Loc(), "All code paths of function do not return a value");

    return true;
}

bool ReturnChecker::ReturnCheckReturn(Return *r, TypeData ret)
{
    if (r->retVal != nullptr && r->retVal->GetType() == ret)
        return true;

    std::string accRetString = ToString(ret);
    TypeData retType = r->retVal->GetType();
    std::string candRetString = ToString(retType);

    ReturnError(r->Loc(), "Type of return value " + candRetString + " does not match the expected return type " + accRetString);
    return false;
}
//-------------------------------VISITOR-------------------------------//

bool ExprStmt::DoesReturn(TypeData)
{
    return false;
}

bool DeclaredVar::DoesReturn(TypeData)
{
    return false;
}

bool ArrayDecl::DoesReturn(TypeData)
{
    return false;
}

bool Block::DoesReturn(TypeData ret)
{
    return ReturnChecker::ReturnCheckBlock(this, ret);
}

bool IfStmt::DoesReturn(TypeData ret)
{
    return ReturnChecker::ReturnCheckIfStmt(this, ret);
}

bool WhileStmt::DoesReturn(TypeData ret)
{
    return ReturnChecker::ReturnCheckWhileStmt(this, ret);
}

bool FuncDecl::DoesReturn(TypeData ret)
{
    return ReturnChecker::ReturnCheckFuncDecl(this, ret);
}

bool Return::DoesReturn(TypeData ret)
{
    return ReturnChecker::ReturnCheckReturn(this, ret);
}