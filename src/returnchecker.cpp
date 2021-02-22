#include "returnchecker.h"

void ReturnChecker::ReturnError(Token loc, std::string err)
{
    Error e = Error("[RETURN ERROR] On line " + std::to_string(loc.line) + "\n" + err + "\n");
    throw e;
}

bool ReturnChecker::ReturnCheckBlock(Block *b, TypeData ret)
{
    for (auto &s : b->stmts)
    {
        try
        {
            if (s->DoesReturn(ret, *this))
                return true;
        }
        catch (const std::exception &e)
        {
            hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }
    return false;
}

bool ReturnChecker::ReturnCheckIfStmt(IfStmt *is, TypeData ret)
{

    bool ans = false;

    Literal *lCond = dynamic_cast<Literal *>(is->cond.get());
    TypeData b = {false, 3};

    if (lCond && lCond->GetType() == b && lCond->as.b)
        ans = is->thenBranch->DoesReturn(ret, *this);

    if (ans)
        return true;

    if (is->elseBranch == nullptr)
        return false;

    return is->thenBranch->DoesReturn(ret, *this) && is->elseBranch->DoesReturn(ret, *this);
}

bool ReturnChecker::ReturnCheckWhileStmt(WhileStmt *ws, TypeData ret)
{
    Literal *lCond = dynamic_cast<Literal *>(ws->cond.get());
    TypeData b = {false, 3};

    if (lCond && lCond->GetType() == b && lCond->as.b)
        return ws->body->DoesReturn(ret, *this);
    return false;
}

bool ReturnChecker::ReturnCheckFuncDecl(FuncDecl *fd, TypeData ret)
{
    try
    {
        bool ans = false;
        for (auto &s : fd->body)
        {
            if (s->DoesReturn(ret, *this))
            {
                ans = true;
                break;
            }
        }
        if (!ans)
            ReturnError(fd->Loc(), "All code paths of function do not return a value");
    }
    catch (const std::exception &e)
    {
        hadError = true;
        std::cerr << e.what() << std::endl;
    }
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

bool ExprStmt::DoesReturn(TypeData, ReturnChecker &)
{
    return false;
}

bool DeclaredVar::DoesReturn(TypeData, ReturnChecker &)
{
    return false;
}

bool Block::DoesReturn(TypeData ret, ReturnChecker &rc)
{
    return rc.ReturnCheckBlock(this, ret);
}

bool IfStmt::DoesReturn(TypeData ret, ReturnChecker &rc)
{
    return rc.ReturnCheckIfStmt(this, ret);
}

bool WhileStmt::DoesReturn(TypeData ret, ReturnChecker &rc)
{
    return rc.ReturnCheckWhileStmt(this, ret);
}

bool FuncDecl::DoesReturn(TypeData ret, ReturnChecker &rc)
{
    return rc.ReturnCheckFuncDecl(this, ret);
}

bool Return::DoesReturn(TypeData ret, ReturnChecker &rc)
{
    return rc.ReturnCheckReturn(this, ret);
}