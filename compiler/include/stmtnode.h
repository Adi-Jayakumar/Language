#pragma once
#include "exprnode.h"
#include <vector>

class PostConditionGenerator;

enum class StmtKind
{
    EXPR_STMT,
    DECLARED_VAR,
    BLOCK,
    IF_STMT,
    WHILE_STMT,
    FUNC_DECL,
    RETURN,
    STRUCT_DECL,
    IMPORT_STMT,
    BREAK,
    THROW,
    TRY_CATCH
};

class Stmt
{
public:
    StmtKind kind;
    Token loc;
    Token Loc() { return loc; };
    // prints the node - implemented in ASTPrinter.cpp
    virtual void Print(ASTPrinter &p) = 0;
    virtual void Analyse(StaticAnalyser &sa) = 0;
    // compiles the node - implemented in Compiler.cpp
    virtual void NodeCompile(Compiler &c) = 0;
    virtual void GeneratePost(PostConditionGenerator &v) = 0;
};

class ExprStmt : public Stmt
{
public:
    SP<Expr> exp;
    ExprStmt(const SP<Expr> &_exp, const Token &_loc)
    {
        kind = StmtKind::EXPR_STMT;
        loc = _loc;
        exp = _exp;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class DeclaredVar : public Stmt
{
public:
    TypeData t;
    std::string name;
    SP<Expr> value;
    DeclaredVar(const TypeData &_t, const std::string &_name, const SP<Expr> &_value, Token _loc)
    {
        kind = StmtKind::DECLARED_VAR;
        loc = _loc;
        t = _t;
        name = _name;
        value = _value;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Block : public Stmt
{
public:
    std::vector<SP<Stmt>> stmts;
    Block(const Token &_loc)
    {
        kind = StmtKind::BLOCK;
        loc = _loc;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class IfStmt : public Stmt
{
public:
    SP<Expr> cond;
    SP<Stmt> thenBranch;
    SP<Stmt> elseBranch;
    IfStmt(const SP<Expr> &_cond, const SP<Stmt> &_thenBranch, const SP<Stmt> &_elseBranch, const Token &_loc)
    {
        kind = StmtKind::IF_STMT;
        loc = _loc;
        cond = _cond;
        thenBranch = _thenBranch;
        elseBranch = _elseBranch;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class WhileStmt : public Stmt
{
public:
    SP<Expr> cond;
    SP<Stmt> body;
    Token loc;

    WhileStmt(const SP<Expr> &_cond, const SP<Stmt> &_body, const Token &_loc)
    {
        kind = StmtKind::WHILE_STMT;
        loc = _loc;
        cond = _cond;
        body = _body;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class FuncDecl : public Stmt
{
public:
    TypeData ret;
    std::string name;
    std::vector<std::pair<TypeData, std::string>> params;

    std::vector<SP<Stmt>> body;
    std::vector<SP<Expr>> preConds;

    std::vector<std::pair<TypeData, std::string>> templates;

    SP<Expr> postCond;

    FuncDecl(TypeData _ret,
             std::string &_name,
             std::vector<std::pair<TypeData, std::string>> &_params,
             std::vector<SP<Stmt>> &_body,
             std::vector<SP<Expr>> &_preConds,
             SP<Expr> &_postCond,
             Token _loc)
    {
        kind = StmtKind::FUNC_DECL;
        loc = _loc;
        ret = _ret;
        name = _name;
        params = _params;
        body = _body;
        preConds = _preConds;
        postCond = _postCond;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Return : public Stmt
{
public:
    SP<Expr> retVal;

    Return(const SP<Expr> &_retVal, const Token &_loc)
    {
        kind = StmtKind::RETURN;
        loc = _loc;
        retVal = _retVal;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class StructDecl : public Stmt
{
public:
    std::string name;
    TypeData parent = VOID_TYPE;
    std::vector<SP<Stmt>> decls;
    StructDecl(const std::string &_name, const TypeData &_parent, const std::vector<SP<Stmt>> &_decls, const Token &_loc)
    {
        kind = StmtKind::STRUCT_DECL;
        loc = _loc;
        name = _name;
        parent = _parent;
        decls = _decls;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

/*
import math, physics
from latin import nominative, accusative
i.e. import --> comma seperated list of libraries <---- imports everything
i.e. from --> single module name import comma seperated list of symbols <---- only imports those symbols
*/
class ImportStmt : public Stmt
{
public:
    std::vector<std::string> libraries;
    ImportStmt(const std::vector<std::string> &_libraries, const Token &_loc)
    {
        kind = StmtKind::IMPORT_STMT;
        loc = _loc;
        libraries = _libraries;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Break : public Stmt
{
public:
    Break(Token _loc)
    {
        kind = StmtKind::BREAK;
        loc = _loc;
    };
    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class Throw : public Stmt
{
public:
    SP<Expr> exp;
    Throw(const SP<Expr> &_exp, const Token &_loc)
    {
        kind = StmtKind::THROW;
        loc = _loc;
        exp = _exp;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};

class TryCatch : public Stmt
{
public:
    SP<Stmt> tryClause, catchClause;
    std::pair<TypeData, std::string> catchVar;

    TryCatch(const SP<Stmt> &_tryClause,
             const SP<Stmt> &_catchClause,
             const std::pair<TypeData, std::string> &_catchVar,
             const Token &_loc)
    {
        kind = StmtKind::TRY_CATCH;
        loc = _loc;
        tryClause = _tryClause;
        catchClause = _catchClause;
        catchVar = _catchVar;
    };

    void Print(ASTPrinter &p) override;
    void Analyse(StaticAnalyser &sa) override;
    void NodeCompile(Compiler &c) override;
    void GeneratePost(PostConditionGenerator &pc) override;
};