#pragma once
#include "stmtnode.h"
#include "symboltable.h"
#include <iostream>
#include <sstream>

class ASTPrinter
{
public:
    std::ostringstream out;
    bool print_types;
    size_t depth;
    std::string tab;
    SymbolTable symbols;

    ASTPrinter(bool _print_types, std::string _tab, SymbolTable &_symbols) : print_types(_print_types), depth(0), tab(_tab), symbols(_symbols){};
    ASTPrinter(bool _printTypes, SymbolTable &_symbols) : print_types(_printTypes), depth(0), tab("\t"), symbols(_symbols){};
    ASTPrinter(std::string _tab, SymbolTable &_symbols) : print_types(false), depth(0), tab(_tab), symbols(_symbols){};

    void Flush()
    {
        std::cout << out.str() << std::endl;
        Clear();
    }

    void Clear()
    {
        out.str("");
        out.clear();
    }

    void NewLine()
    {
        out << '\n';
        for (size_t i = 0; i < depth; i++)
            out << tab;
    }

    void Log(const std::string &text)
    {
        out << text;
        NewLine();
    }

    void PrintTemplateHeader(const std::vector<std::pair<TypeData, std::string>> &templates);

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