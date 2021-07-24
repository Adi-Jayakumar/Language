#include "stmtnode.h"

bool IsConstant(std::shared_ptr<Expr> e);

struct VariableValue
{
    size_t depth;
    TypeData type;
    std::string name;
    std::shared_ptr<Expr> value;
    VariableValue(size_t _depth, TypeData _type, std::string _name, std::shared_ptr<Expr> _value) : depth(_depth), type(_type), name(_name), value(_value){};
};

class ConstantPropagator
{
public:
    size_t depth;
    std::vector<VariableValue> stack;
    ConstantPropagator() = default;

    std::shared_ptr<Expr> GetVariableValue(TypeData type, std::string name);
    void SetVariableValue(TypeData type, std::string name, std::shared_ptr<Expr> val);
    void ClearCurrentDepth();

    // expression interface
    std::shared_ptr<Expr> PropagateUnary(Unary *u);
    std::shared_ptr<Expr> PropagateBinary(Binary *b);
    std::shared_ptr<Expr> PropagateAssign(Assign *a);
    std::shared_ptr<Expr> PropagateVarReference(VarReference *vr);
    std::shared_ptr<Expr> PropagateFunctionCall(FunctionCall *fc);
    std::shared_ptr<Expr> PropagateArrayIndex(ArrayIndex *ai);
    std::shared_ptr<Expr> PropagateBracedInitialiser(BracedInitialiser *ia);
    std::shared_ptr<Expr> PropagateDynamicAllocArray(DynamicAllocArray *da);
    std::shared_ptr<Expr> PropagateFieldAccess(FieldAccess *fa);
    std::shared_ptr<Expr> PropagateTypeCast(TypeCast *gf);

    // statment interface
    void PropagateExprStmt(ExprStmt *es);
    void PropagateDeclaredVar(DeclaredVar *v);
    void PropagateBlock(Block *b);
    void PropagateIfStmt(IfStmt *i);
    void PropagateWhileStmt(WhileStmt *ws);
    void PropagateFuncDecl(FuncDecl *fd);
    void PropagateReturn(Return *r);
    void PropagateStructDecl(StructDecl *sd);
    void PropagateImportStmt(ImportStmt *is);
    void PropagateThrow(Throw *t);
    void PropagateTryCatch(TryCatch *tc);
};