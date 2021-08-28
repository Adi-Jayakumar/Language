#include "stmtnode.h"

ExprStmt::ExprStmt(SP<Expr> _exp, Token _loc)
{
    kind = StmtKind::EXPR_STMT;
    exp = std::move(_exp);
    loc = _loc;
}

Token ExprStmt::Loc()
{
    return loc;
}

DeclaredVar::DeclaredVar(TypeData _t, std::string _name, SP<Expr> _value, Token _loc)
{
    kind = StmtKind::DECLARED_VAR;
    t = _t;
    name = _name;
    value = std::move(_value);
    loc = _loc;
}

Token DeclaredVar::Loc()
{
    return loc;
}

Block::Block(Token _loc)
{
    kind = StmtKind::BLOCK;
    loc = _loc;
}

Token Block::Loc()
{
    return loc;
}

IfStmt::IfStmt(SP<Expr> _cond, SP<Stmt> _thenBranch, SP<Stmt> _elseBranch, Token _loc)
{
    kind = StmtKind::IF_STMT;
    cond = _cond;
    thenBranch = _thenBranch;
    elseBranch = _elseBranch;
    loc = _loc;
}

Token IfStmt::Loc()
{
    return loc;
}

WhileStmt::WhileStmt(SP<Expr> _cond, SP<Stmt> _body, Token _loc)
{
    kind = StmtKind::WHILE_STMT;
    cond = _cond;
    body = _body;
    loc = _loc;
}

Token WhileStmt::Loc()
{
    return loc;
}

FuncDecl::FuncDecl(TypeData _ret,
                   std::string &_name,
                   std::vector<std::pair<TypeData, std::string>> &_params,
                   std::vector<SP<Stmt>> &_body,
                   std::vector<SP<Expr>> &_preConds,
                   SP<Expr> &_postCond,
                   Token _loc)
{
    kind = StmtKind::FUNC_DECL;
    ret = _ret;
    name = _name;

    params = _params;

    body = _body;

    preConds = _preConds;
    postCond = _postCond;
    loc = _loc;
}

Token FuncDecl::Loc()
{
    return loc;
}

Return::Return(SP<Expr> _retVal, Token _loc)
{
    kind = StmtKind::RETURN;
    retVal = _retVal;
    loc = _loc;
}

Token Return::Loc()
{
    return loc;
}

StructDecl::StructDecl(std::string &_name, TypeData &_parent, std::vector<SP<Stmt>> &_decls, Token _loc)
{
    kind = StmtKind::STRUCT_DECL;
    name = _name;
    parent = _parent;
    decls = _decls;
    loc = _loc;
}

Token StructDecl::Loc()
{
    return loc;
}

ImportStmt::ImportStmt(std::vector<std::string> &_libraries, Token &_loc)
{
    kind = StmtKind::IMPORT_STMT;
    libraries = _libraries;
    loc = _loc;
}

Token ImportStmt::Loc()
{
    return loc;
}

Break::Break(Token _loc)
{
    kind = StmtKind::BREAK;
    loc = _loc;
}

Token Break::Loc()
{
    return loc;
}

Throw::Throw(SP<Expr> &_exp, Token &_loc)
{
    kind = StmtKind::THROW;
    exp = _exp;
    loc = _loc;
}

Token Throw::Loc()
{
    return loc;
}

TryCatch::TryCatch(SP<Stmt> &_tryClause, SP<Stmt> &_catchClause, std::pair<TypeData, std::string> &_catchVar, Token &_loc)
{
    kind = StmtKind::TRY_CATCH;
    tryClause = _tryClause;
    catchClause = _catchClause;
    catchVar = _catchVar;
    loc = _loc;
}

Token TryCatch::Loc()
{
    return loc;
}