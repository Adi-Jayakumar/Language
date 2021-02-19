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

size_t TypeChecker::ResolveVariable(std::string &name, Token loc)
{
    size_t varIndex = SIZE_MAX;
    if (isInFunc)
    {
        varIndex = CheckVariablesInFunction(name);
        if (varIndex == SIZE_MAX)
        {
            for (size_t i = vars.size() - 1; (int)i >= 0; i--)
                if (vars[i].name == name)
                {
                    varIndex = i;
                    break;
                }
        }
    }
    else
    {
        for (size_t i = vars.size() - 1; (int)i >= 0; i--)
            if (vars[i].name == name)
            {
                varIndex = i;
                break;
            }
    }
    if (varIndex == SIZE_MAX)
        TypeError(loc, "Variable '" + name + "' has not been defined yet");

    return varIndex;
}

size_t TypeChecker::CheckVariablesInFunction(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= (int)funcVarBegin; i--)
    {
        if ((vars[i].depth == depth) && (vars[i].name.length() == name.length()) && (vars[i].name == name))
            return i;
    }
    return SIZE_MAX;
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

TypeID TypeChecker::ResolveFunction(std::string &name, std::vector<TypeID> &argtypes)
{
    for (size_t i = funcs.size() - 1; (int)i >= 0; i--)
    {
        if ((argtypes.size() == funcs[i].argtypes.size()) && (name.length() == funcs[i].name.length()) && (name == funcs[i].name))
        {
            bool doesMatch = true;
            for (size_t j = 0; j < argtypes.size(); j++)
            {
                if (argtypes[j] != funcs[i].argtypes[j])
                {
                    doesMatch = false;
                    break;
                }
            }
            if (doesMatch)
                return i;
        }
    }
    return UINT8_MAX;
}

//-----------------EXPRESSIONS---------------------//

TypeID TypeChecker::TypeOfLiteral(Literal *l)
{
    return l->typeID;
}

TypeID TypeChecker::TypeOfUnary(Unary *u)
{
    TypeID opType = u->right->Type(*this);
    TypeInfo info = {0, u->op.type, opType};

    if (OperatorMap.find(info) != OperatorMap.end())
    {
        u->typeID = OperatorMap.at(info);
        return u->typeID;
    }
    else
        TypeError(u->Loc(), "Cannot use operator: " + ToString(u->op.type) + " on operand of type: " + TypeStringMap.at(opType));

    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfBinary(Binary *b)
{
    if (b == nullptr || b->left == nullptr || b->right == nullptr)
        return UINT8_MAX;

    TypeID lType = b->left->Type(*this);
    TypeID rType = b->right->Type(*this);
    TypeInfo info = {lType, b->op.type, rType};

    if (OperatorMap.find(info) != OperatorMap.end())
    {
        b->typeID = OperatorMap.at(info);
        return b->typeID;
    }
    else
        TypeError(b->Loc(), "Cannot use operator: " + ToString(b->op.type) + " on operands of type: " + TypeStringMap.at(lType) + " and: " + TypeStringMap.at(rType));
    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfAssign(Assign *a)
{
    size_t varIndex = ResolveVariable(a->var->name, a->Loc());
    TypeID varType = vars[varIndex].type;
    TypeID valType = a->val->Type(*this);

    if (varType != valType)
        TypeError(a->Loc(), "Cannot assign " + TypeStringMap[valType] + " to variable of type " + TypeStringMap[varType]);

    // std::cout << "assign varIndex isarray: " << vars[varIndex].isArray << std::endl;
    a->var->isArray = vars[varIndex].isArray;
    a->var->typeID = varType;
    a->typeID = varType;

    return varType;
}

TypeID TypeChecker::TypeOfVarReference(VarReference *vr)
{
    size_t varIndex = ResolveVariable(vr->name, vr->Loc());
    vr->typeID = vars[varIndex].type;
    vr->isArray = vars[varIndex].isArray;
    return vars[varIndex].type;
}

TypeID TypeChecker::TypeOfFunctionCall(FunctionCall *fc)
{
    std::vector<TypeID> argtypes;

    for (auto &e : fc->args)
        argtypes.push_back(e->Type(*this));

    size_t index = ResolveFunction(fc->name, argtypes);

    if (index == UINT8_MAX)
        TypeError(fc->Loc(), "Function: '" + fc->name + "' has not been defined yet");

    if (index > UINT8_MAX)
        TypeError(fc->Loc(), "Cannot have more than " + std::to_string(UINT8_MAX) + " functions");

    fc->typeID = funcs[index].ret;
    return funcs[index].ret;
}

TypeID TypeChecker::TypeOfArrayIndex(ArrayIndex *ai)
{
    return 0;
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
        return UINT8_MAX;

    if (IsVariableInScope(dv->name))
        TypeError(dv->Loc(), "Variable: '" + dv->name + "' has already been defined");

    vars.push_back({dv->tId, dv->name, depth, false});
    if (dv->value == nullptr)
        return dv->tId;
    else
    {
        TypeID varType = dv->tId;
        TypeID valType = dv->value->Type(*this);

        if (valType == varType)
            return valType;
        else
            TypeError(dv->Loc(), "Cannot assign value of type: " + TypeStringMap.at(valType) + " to variable: '" + dv->name + "' of type: " + TypeStringMap.at(varType));
    }
    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfArrayDecl(ArrayDecl *ad)
{
    if (IsVariableInScope(ad->name))
        TypeError(ad->Loc(), "Variable: '" + ad->name + "' has already been defined");

    vars.push_back({ad->elemType, ad->name, depth, true});
    for (auto &e : ad->init)
    {
        TypeID valType = e->Type(*this);
        if (valType != ad->elemType)
            TypeError(ad->Loc(), "Cannot declare an Array<" + TypeStringMap.at(ad->elemType) + "> with a " + TypeStringMap.at(valType));
    }
    return UINT8_MAX;
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
    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfIfStmt(IfStmt *i)
{
    if (i->cond->Type(*this) != 3)
        TypeError(i->Loc(), "Condition of and if statement must have type: bool");
    i->thenBranch->Type(*this);
    if (i->elseBranch != nullptr)
        i->elseBranch->Type(*this);
    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfWhileStmt(WhileStmt *ws)
{
    if (ws->cond->Type(*this) != 3)
        TypeError(ws->Loc(), "Condition of a while statment must have type: bool");

    ws->body->Type(*this);
    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfFuncDecl(FuncDecl *fd)
{
    depth++;
    if (funcs.size() > UINT8_MAX)
        TypeError(fd->loc, "Max number of functions is: " + std::to_string(UINT8_MAX));

    std::vector<TypeID> argtypes;

    for (auto &t : fd->params)
    {
        if (t.type == TokenID::TYPENAME)
            argtypes.push_back(TypeNameMap[t.literal]);
    }

    funcs.push_back({fd->ret, fd->name, argtypes});

    isInFunc = true;
    funcVarBegin = vars.size();

    for (size_t i = 0; i < fd->params.size(); i++)
    {
        TypeID pType = 0;
        std::string pName;
        if (fd->params[i].type == TokenID::TYPENAME)
            pType = TypeNameMap[fd->params[i].literal];
        i++;
        if (fd->params[i].type == TokenID::IDEN)
            pName = fd->params[i].literal;

        bool isArray = false;
        if (fd->params[i].type == TokenID::ARRAY)
            isArray = true;

        vars.push_back({pType, pName, depth, isArray});
    }

    for (auto &s : fd->body)
        s->Type(*this);

    isInFunc = false;
    CleanUpVariables();
    funcVarBegin = 0;
    depth--;
    return UINT8_MAX;
}

TypeID TypeChecker::TypeOfReturn(Return *r)
{
    if (depth == 0)
        TypeError(r->Loc(), "Cannot return from outside of a function");
    if (r->retVal == nullptr)
        return 0;
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

TypeID FunctionCall::Type(TypeChecker &t)
{
    return t.TypeOfFunctionCall(this);
}

TypeID ArrayIndex::Type(TypeChecker &t)
{
    return t.TypeOfArrayIndex(this);
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

TypeID ArrayDecl::Type(TypeChecker &t)
{
    return t.TypeOfArrayDecl(this);
}

TypeID Block::Type(TypeChecker &t)
{
    return t.TypeOfBlock(this);
}

TypeID IfStmt::Type(TypeChecker &t)
{
    return t.TypeOfIfStmt(this);
}

TypeID WhileStmt::Type(TypeChecker &t)
{
    return t.TypeOfWhileStmt(this);
}

TypeID FuncDecl::Type(TypeChecker &t)
{
    return t.TypeOfFuncDecl(this);
}

TypeID Return::Type(TypeChecker &t)
{
    return t.TypeOfReturn(this);
}