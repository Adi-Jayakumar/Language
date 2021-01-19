#include "typechecker.h"

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

void TypeChecker::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + "\n" + err);
    e.Dump();
}

void TypeChecker::TypeCheck(std::shared_ptr<Stmt> &s)
{
    s->Type(*this);
}

TypeID TypeChecker::ResolveVariable(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name)
            return vars[i].type;
    }
    return UINT16_MAX;
}

TypeID TypeChecker::ResolveVariableInScope(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if ((vars[i].depth == depth) && (vars[i].name.length() == name.length()) && (vars[i].name == name))
            return vars[i].type;
    }
    return UINT16_MAX;
}

bool TypeChecker::IsVariableInScope(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].depth == depth && vars[i].name == name)
            return true;
    }
    return false;
}

void TypeChecker::CleanUpVariables()
{
    while (!vars.empty() && vars.back().depth == depth)
        vars.pop_back();
}

//-----------------EXPRESSIONS---------------------//

TypeID TypeChecker::TypeOfLiteral(Literal *l)
{
    if (l == nullptr)
        return UINT16_MAX;
    return l->typeID;
}

TypeID TypeChecker::TypeOfUnary(Unary *u)
{
    if (u == nullptr)
        return 0;
    if (u->right == nullptr)
        return UINT16_MAX;

    TypeID opType = u->right->Type(*this);
    TypeInfo info = {opType, u->op.type, 0};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);
    else
        TypeError(u->Loc(), "Cannot use operator: " + std::to_string(static_cast<uint8_t>(u->op.type)) + " on operand of type: " + std::to_string(opType));

    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfBinary(Binary *b)
{
    if (b == nullptr || b->left == nullptr || b->right == nullptr)
        return UINT16_MAX;

    TypeID lType = b->left->Type(*this);
    TypeID rType = b->right->Type(*this);
    TypeInfo info = {lType, b->op.type, rType};

    if (OperatorMap.find(info) != OperatorMap.end())
        return OperatorMap.at(info);
    else
        TypeError(b->Loc(), "Cannot use operator: " + std::to_string(static_cast<uint8_t>(b->op.type)) + " on operands of type: " + std::to_string(lType) + " and: " + std::to_string(rType));
    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfAssign(Assign *a)
{
    TypeID varType = isInFunc ? ResolveVariableInScope(a->var->name) : ResolveVariable(a->var->name);

    if (varType == UINT16_MAX)
        TypeError(a->var->Loc(), "Variable name: '" + a->var->name + "' has not been defined before");

    // TypeID varType = ResolveVariable(a->var->name);
    TypeID valType = a->val->Type(*this);

    if (varType == valType)
        return varType;
    else
        TypeError(a->Loc(), "Cannot assign value of type: " + std::to_string(valType) + " to variable: '" + a->var->name + "' of type: " + std::to_string(varType));
    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfVarReference(VarReference *vr)
{
    TypeID type = isInFunc ? ResolveVariableInScope(vr->name) : ResolveVariable(vr->name);

    if (type == UINT16_MAX)
        TypeError(vr->Loc(), "Variable name: '" + vr->name + "' has not been defined before");

    return type;
}

//------------------STATEMENTS---------------------//

TypeID TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    if (es == nullptr || es->exp == nullptr)
        return 0;
    return es->exp->Type(*this);
}

TypeID TypeChecker::TypeOfDeclaredVar(DeclaredVar *dv)
{
    if (dv == nullptr)
        return UINT16_MAX;

    if (IsVariableInScope(dv->name))
        TypeError(dv->Loc(), "Variable: '" + dv->name + "' has already been defined");

    vars.push_back({dv->tId, dv->name, depth});
    if (dv->value == nullptr)
        return dv->tId;
    else
    {
        TypeID varType = dv->tId;
        TypeID valType = dv->value->Type(*this);

        if (valType == varType)
            return valType;
        else
            TypeError(dv->Loc(), "Cannot assign value of type: " + std::to_string(valType) + " to variable: '" + dv->name + "' of type: " + std::to_string(varType));
    }
    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfBlock(Block *b)
{
    depth++;
    if (depth == UINT8_MAX)
        TypeError(b->Loc(), "Exceeded maximum number of nested blocks: " + std::to_string(UINT8_MAX));
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s->Type(*this);
    // CleanUpVariables();
    depth--;
    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfIfStmt(IfStmt *i)
{
    if (i->cond->Type(*this) != 3)
        TypeError(i->Loc(), "Condition of and if statement must have type: bool");
    i->thenBranch->Type(*this);
    if (i->elseBranch != nullptr)
        i->elseBranch->Type(*this);
    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfFuncDecl(FuncDecl *fd)
{
    isInFunc = true;

    for (size_t i = 0; i < fd->params.size(); i++)
    {
        TypeID pType;
        std::string pName;
        if (fd->params[i].type == TokenID::TYPENAME)
            pType = TypeNameMap[fd->params[i].literal];
        i++;
        if (fd->params[i].type == TokenID::IDEN)
            pName = fd->params[i].literal;
        vars.push_back({pType, pName, depth});
    }

    for(auto &s : fd->body)
        s->Type(*this);

    isInFunc = false;
    CleanUpVariables();
    return UINT16_MAX;
}

TypeID TypeChecker::TypeOfReturn(Return *r)
{
    return r->retVal->Type(*this);
}

//-----------------EXPRESSIONS---------------------//

TypeID Literal::Type(TypeChecker &t)
{
    return t.TypeOfLiteral(this);
}

TypeID Unary::Type(TypeChecker &t)
{
    return t.TypeOfUnary(this);
}

TypeID Binary::Type(TypeChecker &t)
{
    return t.TypeOfBinary(this);
}

TypeID Assign::Type(TypeChecker &t)
{
    return t.TypeOfAssign(this);
}

TypeID VarReference::Type(TypeChecker &t)
{
    return t.TypeOfVarReference(this);
}

//------------------STATEMENTS---------------------//

TypeID ExprStmt::Type(TypeChecker &t)
{
    return t.TypeOfExprStmt(this);
}

TypeID DeclaredVar::Type(TypeChecker &t)
{
    return t.TypeOfDeclaredVar(this);
}

TypeID Block::Type(TypeChecker &t)
{
    return t.TypeOfBlock(this);
}

TypeID IfStmt::Type(TypeChecker &t)
{
    return t.TypeOfIfStmt(this);
}

TypeID FuncDecl::Type(TypeChecker &t)
{
    return t.TypeOfFuncDecl(this);
}

TypeID Return::Type(TypeChecker &t)
{
    return t.TypeOfReturn(this);
}