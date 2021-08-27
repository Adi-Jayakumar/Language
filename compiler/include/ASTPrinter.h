#pragma once
#include "stmtnode.h"
#include <iostream>
#include <sstream>

class ASTPrinter
{
public:
    std::ostringstream out;
    bool printTypes = false;
    size_t depth = 0;
    std::string tab = "\t";

    ASTPrinter(bool _printTypes, std::string _tab) : printTypes(_printTypes), tab(_tab){};
    ASTPrinter(bool _printTypes) : printTypes(_printTypes){};
    ASTPrinter(std::string _tab) : tab(_tab){};

    void Flush()
    {
        std::cout << out.str() << std::endl;
        Clear();
    }

    void Clear()
    {
        out.clear();
    }

    void NewLine()
    {
        out << '\n';
        for (size_t i = 0; i < depth; i++)
            out << tab;
    }

    // expression printing
    void PrintLiteral(Literal *l);
    void PrintUnary(Unary *u);
    void PrintBinary(Binary *b);
    void PrintAssign(Assign *a);
    void PrintVarReference(VarReference *vr);
    void PrintFunctionCall(FunctionCall *fc);
    void PrintArrayIndex(ArrayIndex *ai);
    void PrintBracedInitialiser(BracedInitialiser *ia);
    void PrintDynamicAllocArray(DynamicAllocArray *da);
    void PrintFieldAccess(FieldAccess *fa);
    void PrintTypeCast(TypeCast *gf);
    void PrintSequence(Sequence *s);

    // statment printing
    void PrintExprStmt(ExprStmt *es);
    void PrintDeclaredVar(DeclaredVar *v);
    void PrintBlock(Block *b);
    void PrintIfStmt(IfStmt *i);
    void PrintWhileStmt(WhileStmt *ws);
    void PrintFuncDecl(FuncDecl *fd);
    void PrintReturn(Return *r);
    void PrintStructDecl(StructDecl *sd);
    void PrintImportStmt(ImportStmt *is);
    void PrintBreak(Break *b);
    void PrintThrow(Throw *t);
    void PrintTryCatch(TryCatch *tc);
}; // namespace ASTPrinter

ASTPrinter &operator<<(ASTPrinter &ast, std::string str);