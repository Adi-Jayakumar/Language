#include "returnchecker.h"

void ReturnChecker::ReturnError(Token loc, std::string err)
{
    Error e = Error("[RETURN ERROR] On line " + std::to_string(loc.line) + "\n" + err);
    e.Dump();
}

bool ReturnChecker::ReturnCheckBlock(Block *b, TypeID ret)
{
    for (auto &s : b->stmts)
    {
        if (s->DoesReturn(ret))
            return true;
    }
    return false;
}

bool ReturnChecker::ReturnCheckIfStmt(IfStmt *is, TypeID ret)
{

    bool ans = false;

    Literal *lCond = dynamic_cast<Literal *>(is->cond.get());
    if (lCond && lCond->GetType() == 3 && lCond->as.b)
        ans =  is->thenBranch->DoesReturn(ret);
    
    if(ans)
        return true;

    if (is->elseBranch == nullptr)
    {
        return false;
    }
    return is->thenBranch->DoesReturn(ret) && is->elseBranch->DoesReturn(ret);
}

bool ReturnChecker::ReturnCheckWhileStmt(WhileStmt *ws, TypeID ret)
{
    Literal *lCond = dynamic_cast<Literal *>(ws->cond.get());
    if (lCond && lCond->GetType() == 3 && lCond->as.b)
        return ws->body->DoesReturn(ret);
    return false;
}

bool ReturnChecker::ReturnCheckFuncDecl(FuncDecl *fd, TypeID ret)
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

bool ReturnChecker::ReturnCheckReturn(Return *r, TypeID ret)
{
    if (r->retVal != nullptr && r->retVal->GetType() == ret)
        return true;

    std::string retType = (r->retVal != nullptr) ? TypeStringMap.at(ret) : "void";

    ReturnError(r->Loc(), "Type of return value " + retType + " does not match the expected return type " + TypeStringMap.at(ret));
    return false;
}
//-------------------------------VISITOR-------------------------------//

bool ExprStmt::DoesReturn(TypeID ret)
{
    return false;
}

bool DeclaredVar::DoesReturn(TypeID ret)
{
    return false;
}

bool Block::DoesReturn(TypeID ret)
{
    return ReturnChecker::ReturnCheckBlock(this, ret);
}

bool IfStmt::DoesReturn(TypeID ret)
{
    return ReturnChecker::ReturnCheckIfStmt(this, ret);
}

bool WhileStmt::DoesReturn(TypeID ret)
{
    return ReturnChecker::ReturnCheckWhileStmt(this, ret);
}

bool FuncDecl::DoesReturn(TypeID ret)
{
    return ReturnChecker::ReturnCheckFuncDecl(this, ret);
}

bool Return::DoesReturn(TypeID ret)
{
    return ReturnChecker::ReturnCheckReturn(this, ret);
}