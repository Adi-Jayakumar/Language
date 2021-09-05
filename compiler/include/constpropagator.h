#include "stmtnode.h"
#include "symboltable.h"

struct VarRef
{
    TypeData type;
    std::string name;
    SP<Expr> val;
    size_t depth;

    VarRef(const TypeData &_type,
           const std::string &_name,
           const SP<Expr> &_val,
           const size_t &_depth) : type(_type),
                                   name(_name),
                                   val(_val),
                                   depth(_depth){};

    VarRef(const TypeData &_type,
           const std::string &_name,
           const size_t &_depth) : type(_type),
                                   name(_name),
                                   val(nullptr),
                                   depth(_depth){};
};

class ConstantMap
{
    std::vector<VarRef> vlist;

public:
    size_t depth;
    ConstantMap() = default;

    void AddVar(const TypeData &type, const std::string &name, const SP<Expr> &val)
    {
        vlist.push_back(VarRef(type, name, val, depth));
    }

    SP<Expr> GetVarVal(const TypeData &type, const std::string &name)
    {
        for (size_t i = vlist.size() - 1; (int)i >= 0; i--)
        {
            if (vlist[i].type == type && vlist[i].name == name)
                return vlist[i].val;
        }
        return nullptr;
    }

    void SetVarVal(const TypeData &type, const std::string &name, const SP<Expr> &val)
    {
        for (size_t i = vlist.size() - 1; (int)i >= 0; i--)
        {
            if (vlist[i].type == type && vlist[i].name == name)
                vlist[i].val = val;
        }
        return;
    }

    void RemoveVarVal(const TypeData &type, const std::string &name)
    {
        for (size_t i = vlist.size() - 1; (int)i >= 0; i--)
        {
            if (vlist[i].type == type && vlist[i].name == name)
                vlist[i].val = nullptr;
        }
        return;
    }

    void ClearCurrentDepth()
    {
        if (vlist.size() == 0)
            return;

        while (vlist.back().depth == depth)
        {
            if (vlist.size() == 1 && vlist[0].depth == depth)
            {
                vlist.clear();
                return;
            }
            vlist.pop_back();
        }
    }
};

// ConstantPropagator assumes that the parse tree has been
// validated by the StaticAnalyser
class ConstantPropagator
{
    SymbolTable Symbols;
    ConstantMap Constants;

public:
    bool didPropagate;
    ConstantPropagator() = default;

    void Propagate(std::vector<SP<Stmt>> &program);

    SP<Expr> PropagateExpression(SP<Expr> &expr);
    void PropagateStatement(SP<Stmt> &stmt);
};