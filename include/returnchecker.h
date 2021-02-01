#pragma once
#include "stmtnode.h"
#include "perror.h"


// checking that all codepaths of a function return the required type
// done after typechecking so that we know the types of all nodes
namespace ReturnChecker
{
    void ReturnError(Token loc, std::string err);
    // statment return checking
    bool ReturnCheckBlock(Block *b, TypeID ret);
    bool ReturnCheckIfStmt(IfStmt *i, TypeID ret);
    bool ReturnCheckWhileStmt(WhileStmt *ws, TypeID ret);
    bool ReturnCheckFuncDecl(FuncDecl *fd, TypeID ret);
    bool ReturnCheckReturn(Return *r, TypeID ret);
} // namespace ReturnChecker
