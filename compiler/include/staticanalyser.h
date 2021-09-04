#include "nodesubstitution.h"
#include "stmtnode.h"
#include "symboltable.h"

class StaticAnalyser
{
    SymbolTable Symbols;
    std::vector<SP<Stmt>> program;
    size_t parseIndex;
    bool verifying = false;
    std::pair<SP<Expr>, SP<Expr>> verExp;

public:
    StaticAnalyser() = default;

    void StaticAnalysisError(Token loc, std::string err);
    void TypeError(Token loc, std::string err);
    void SymbolError(Token loc, std::string err);

    void Analyse(std::vector<SP<Stmt>> &program);
    void AnalysePost(std::vector<std::vector<SP<Expr>>> &post, const TypeData &ret);

    void SetVerify() { verifying = true; };
    void SetNormal() { verifying = false; };

    // expression analysis
    TypeData AnalyseLiteral(Literal *l);
    TypeData AnalyseUnary(Unary *u);
    TypeData AnalyseBinary(Binary *b);
    TypeData AnalyseAssign(Assign *a);
    TypeData AnalyseVarReference(VarReference *vr);
    TypeData AnalyseFunctionCall(FunctionCall *fc);
    TypeData AnalyseArrayIndex(ArrayIndex *ai);
    TypeData AnalyseBracedInitialiser(BracedInitialiser *ia);
    TypeData AnalyseDynamicAllocArray(DynamicAllocArray *da);
    TypeData AnalyseFieldAccess(FieldAccess *fa);
    TypeData AnalyseTypeCast(TypeCast *tc);
    TypeData AnalyseSequence(Sequence *s);

    // statement analysis
    void AnalyseExprStmt(ExprStmt *es);
    void AnalyseDeclaredVar(DeclaredVar *dv);
    void AnalyseBlock(Block *b);
    void AnalyseIfStmt(IfStmt *i);
    void AnalyseWhileStmt(WhileStmt *ws);
    void AnalyseFuncDecl(FuncDecl *fd);
    void AnalyseReturn(Return *r);
    void AnalyseStructDecl(StructDecl *sd);
    void AnalyseImportStmt(ImportStmt *is);
    void AnalyseBreak(Break *b);
    void AnalyseThrow(Throw *t);
    void AnalyserTryCatch(TryCatch *tc);
};