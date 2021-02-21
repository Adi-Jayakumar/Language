#include "typechecker.h"

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

size_t TypeChecker::ResolveFunction(std::string &name, std::vector<TypeData> &argtypes)
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

TypeData TypeChecker::TypeOfLiteral(Literal *l)
{
    return l->t;
}

TypeData TypeChecker::TypeOfUnary(Unary *u)
{
    TypeData opType = u->right->Type(*this);
    TypeInfo info = {{false, 0}, u->op.type, opType};

    if (OperatorMap.find(info) != OperatorMap.end())
    {
        u->t = OperatorMap.at(info);
        return u->t;
    }
    else
        TypeError(u->Loc(), "Cannot use operator: " + ToString(u->op.type) + " on operand of type: " + ToString(opType));

    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfBinary(Binary *b)
{
    TypeData lType = b->left->Type(*this);
    TypeData rType = b->right->Type(*this);
    TypeInfo info = {lType, b->op.type, rType};

    if (OperatorMap.find(info) != OperatorMap.end())
    {
        b->t = OperatorMap.at(info);
        return b->t;
    }
    else
        TypeError(b->Loc(), "Cannot use operator: " + ToString(b->op.type) + " on operands of type: " + ToString(lType) + " and: " + ToString(rType));
    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfAssign(Assign *a)
{
    VarReference *targetAsVr = dynamic_cast<VarReference *>(a->target.get());

    TypeData valType = a->val->Type(*this);

    if (targetAsVr != nullptr)
    {
        size_t varIndex = ResolveVariable(targetAsVr->name, a->Loc());
        TypeData varType = vars[varIndex].type;

        if (varType != valType)
            TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(varType));

        // std::cout << "assign varIndex isarray: " << vars[varIndex].isArray << std::endl;
        targetAsVr->isArray = vars[varIndex].type.isArray;
        targetAsVr->t = varType;
        a->t = varType;
        return varType;
    }

    ArrayIndex *targetAsAi = dynamic_cast<ArrayIndex *>(a->target.get());
    TypeData targetType = targetAsAi->Type(*this);
    if (targetType != valType)
        TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(targetType));
    return targetType;
}

TypeData TypeChecker::TypeOfVarReference(VarReference *vr)
{
    size_t varIndex = ResolveVariable(vr->name, vr->Loc());
    vr->t = vars[varIndex].type;
    vr->isArray = vars[varIndex].type.isArray;
    return vars[varIndex].type;
}

TypeData TypeChecker::TypeOfFunctionCall(FunctionCall *fc)
{
    std::vector<TypeData> argtypes;

    for (auto &e : fc->args)
        argtypes.push_back(e->Type(*this));

    size_t index = ResolveFunction(fc->name, argtypes);

    if (index == UINT8_MAX)
        TypeError(fc->Loc(), "Function: '" + fc->name + "' has not been defined yet");

    if (index > UINT8_MAX)
        TypeError(fc->Loc(), "Cannot have more than " + std::to_string(UINT8_MAX) + " functions");

    fc->t = funcs[index].ret;
    return funcs[index].ret;
}

TypeData TypeChecker::TypeOfArrayIndex(ArrayIndex *ai)
{
    size_t varIndex = ResolveVariable(ai->name, ai->Loc());

    if (!vars[varIndex].type.isArray)
        TypeError(ai->Loc(), "Cannot index into variable '" + ai->name + "' since it is of type " + ToString(vars[varIndex].type));

    TypeData indexType = ai->index->Type(*this);
    TypeData intType = {false, 1};

    if (indexType != intType)
        TypeError(ai->Loc(), "Index into an array must have type int not " + ToString(indexType));

    ai->t = vars[varIndex].type;
    ai->t.isArray = false;

    return ai->t;
}

//------------------STATEMENTS---------------------//

TypeData TypeChecker::TypeOfExprStmt(ExprStmt *es)
{
    return es->exp->Type(*this);
}

TypeData TypeChecker::TypeOfDeclaredVar(DeclaredVar *dv)
{
    if (IsVariableInScope(dv->name))
        TypeError(dv->Loc(), "Variable: '" + dv->name + "' has already been defined");

    vars.push_back({dv->t, dv->name, depth});
    if (dv->value == nullptr)
        return dv->t;
    else
    {
        TypeData varType = dv->t;
        TypeData valType = dv->value->Type(*this);

        if (valType == varType)
            return valType;
        else
            TypeError(dv->Loc(), "Cannot assign value of type: " + ToString(valType) + " to variable: '" + dv->name + "' of type: " + ToString(varType));
    }
    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfArrayDecl(ArrayDecl *ad)
{
    if (IsVariableInScope(ad->name))
        TypeError(ad->Loc(), "Variable: '" + ad->name + "' has already been defined");

    TypeData arrT = ad->elemType;
    arrT.isArray = true;

    vars.push_back({arrT, ad->name, depth});
    for (auto &e : ad->init)
    {
        TypeData valType = e->Type(*this);
        if (valType.type != ad->elemType.type)
            TypeError(ad->Loc(), "Cannot declare an Array<" + ToString(ad->elemType) + "> with a " + ToString(valType));
    }
    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfBlock(Block *b)
{
    depth++;
    if (depth == UINT8_MAX)
        TypeError(b->Loc(), "Exceeded maximum number of nested blocks: " + std::to_string(UINT8_MAX));
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s->Type(*this);
    // CleanUpVariables();
    depth--;
    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfIfStmt(IfStmt *i)
{
    TypeData boolType = {false, 3};

    if (i->cond->Type(*this) != boolType)
        TypeError(i->Loc(), "Condition of and if statement must have type: bool");

    i->thenBranch->Type(*this);

    if (i->elseBranch != nullptr)
        i->elseBranch->Type(*this);

    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfWhileStmt(WhileStmt *ws)
{
    TypeData bType = {false, 3};
    if (ws->cond->Type(*this) != bType)
        TypeError(ws->Loc(), "Condition of a while statment must have type: bool");

    ws->body->Type(*this);
    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfFuncDecl(FuncDecl *fd)
{
    depth++;
    if (funcs.size() > UINT8_MAX)
        TypeError(fd->loc, "Max number of functions is: " + std::to_string(UINT8_MAX));


    funcs.push_back({fd->ret, fd->name, fd->argtypes});

    isInFunc = true;
    funcVarBegin = vars.size();

    if (fd->argtypes.size() != fd->paramIdentifiers.size())
    {
        std::cout << "SOMETHING WENT WRONG HERE" << std::endl;
        exit(14);
    }

    for (size_t j = 0; j < fd->argtypes.size(); j++)
        vars.push_back({fd->argtypes[j], fd->paramIdentifiers[j], depth});

    for (auto &s : fd->body)
        s->Type(*this);

    isInFunc = false;
    CleanUpVariables();
    funcVarBegin = 0;
    depth--;
    return {false, UINT8_MAX};
}

TypeData TypeChecker::TypeOfReturn(Return *r)
{
    if (depth == 0)
        TypeError(r->Loc(), "Cannot return from outside of a function");
    if (r->retVal == nullptr)
        return {false, 0};
    return r->retVal->Type(*this);
}

//-----------------EXPRESSIONS---------------------//

TypeData Literal::Type(TypeChecker &t)
{
    return t.TypeOfLiteral(this);
}

TypeData Unary::Type(TypeChecker &t)
{
    return t.TypeOfUnary(this);
}

TypeData Binary::Type(TypeChecker &t)
{
    return t.TypeOfBinary(this);
}

TypeData Assign::Type(TypeChecker &t)
{
    return t.TypeOfAssign(this);
}

TypeData VarReference::Type(TypeChecker &t)
{
    return t.TypeOfVarReference(this);
}

TypeData FunctionCall::Type(TypeChecker &t)
{
    return t.TypeOfFunctionCall(this);
}

TypeData ArrayIndex::Type(TypeChecker &t)
{
    return t.TypeOfArrayIndex(this);
}

//------------------STATEMENTS---------------------//

TypeData ExprStmt::Type(TypeChecker &t)
{
    return t.TypeOfExprStmt(this);
}

TypeData DeclaredVar::Type(TypeChecker &t)
{
    return t.TypeOfDeclaredVar(this);
}

TypeData ArrayDecl::Type(TypeChecker &t)
{
    return t.TypeOfArrayDecl(this);
}

TypeData Block::Type(TypeChecker &t)
{
    return t.TypeOfBlock(this);
}

TypeData IfStmt::Type(TypeChecker &t)
{
    return t.TypeOfIfStmt(this);
}

TypeData WhileStmt::Type(TypeChecker &t)
{
    return t.TypeOfWhileStmt(this);
}

TypeData FuncDecl::Type(TypeChecker &t)
{
    return t.TypeOfFuncDecl(this);
}

TypeData Return::Type(TypeChecker &t)
{
    return t.TypeOfReturn(this);
}