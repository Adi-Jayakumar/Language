#include "stmtnode.h"
#include "symboltable.h"

class StaticAnalyser
{
    SymbolTable Symbols;

public:
    void StaticAnalysisError(Token loc, std::string err);
    void TypeError(Token loc, std::string err);
    void SymbolError(Token loc, std::string err);

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