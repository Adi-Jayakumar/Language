#pragma once
#include "stmtnode.h"
#include "perror.h"


// checking that all codepaths of a function return the required type
// done after typechecking so that we know the types of all nodes
namespace ReturnChecker
{
    void ReturnError(Token loc, std::string err);
    // statment return checking
    bool ReturnCheckBlock(Block *b, TypeData ret);
    bool ReturnCheckIfStmt(IfStmt *i, TypeData ret);
    bool ReturnCheckWhileStmt(WhileStmt *ws, TypeData ret);
    bool ReturnCheckFuncDecl(FuncDecl *fd, TypeData ret);
    bool ReturnCheckReturn(Return *r, TypeData ret);
} // namespace ReturnChecker
