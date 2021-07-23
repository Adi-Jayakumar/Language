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
    std::shared_ptr<Expr> PropagateUnary(Unary *u, bool &didSimp);
    std::shared_ptr<Expr> PropagateBinary(Binary *b, bool &didSimp);
    std::shared_ptr<Expr> PropagateAssign(Assign *a, bool &didSimp);
    std::shared_ptr<Expr> PropagateVarReference(VarReference *vr, bool &didSimp);
    std::shared_ptr<Expr> PropagateFunctionCall(FunctionCall *fc, bool &didSimp);
    std::shared_ptr<Expr> PropagateArrayIndex(ArrayIndex *ai, bool &didSimp);
    std::shared_ptr<Expr> PropagateBracedInitialiser(BracedInitialiser *ia, bool &didSimp);
    std::shared_ptr<Expr> PropagateDynamicAllocArray(DynamicAllocArray *da, bool &didSimp);
    std::shared_ptr<Expr> PropagateFieldAccess(FieldAccess *fa, bool &didSimp);
    std::shared_ptr<Expr> PropagateTypeCast(TypeCast *gf, bool &didSimp);

    // statment interface
    void PropagateExprStmt(ExprStmt *es, bool &didSimp);
    void PropagateDeclaredVar(DeclaredVar *v, bool &didSimp);
    void PropagateBlock(Block *b, bool &didSimp);
    void PropagateIfStmt(IfStmt *i, bool &didSimp);
    void PropagateWhileStmt(WhileStmt *ws, bool &didSimp);
    void PropagateFuncDecl(FuncDecl *fd, bool &didSimp);
    void PropagateReturn(Return *r, bool &didSimp);
    void PropagateStructDecl(StructDecl *sd, bool &didSimp);
    void PropagateImportStmt(ImportStmt *is, bool &didSimp);
    void PropagateThrow(Throw *t, bool &didSimp);
    void PropagateTryCatch(TryCatch *tc, bool &didSimp);
};