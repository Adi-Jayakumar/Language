#include "typechecker.h"

void TypeChecker::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + '\n' + err + '\n');
    throw e;
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

bool TypeChecker::IsTruthy(const TypeData &cond)
{
    if (cond.isArray)
        return false;

    return (cond.type == 1) || (cond.type == 2) || (cond.type == 3);
}

bool TypeChecker::CanAssign(const TypeData &varType, const TypeData &valType)
{
    if (varType.isArray != valType.isArray)
        return false;

    if (varType.type == 6 || valType.type == 6)
        return true;
    else if (varType.type == 1 && valType.type == 2)
        return true;
    else if (varType.type == 2 && valType.type == 1)
        return true;
    return varType == valType;
}

bool TypeChecker::MatchNativeArguments(std::vector<TypeData> &actual, std::vector<TypeData> &supplied)
{
    if (actual.size() == 0 && supplied.size() != 0)
        return false;

    TypeData matchMoreThanOne = {true, 0};
    if (actual.size() == 1 && actual[0] == matchMoreThanOne && supplied.size() > 0)
        return true;

    TypeData matchOne = {false, 0};
    if (supplied.size() == 1 && actual.size() == 1 && actual[0] == matchOne)
        return true;

    if (actual.size() != supplied.size())
        return false;

    bool areSame = true;
    for (size_t i = 0; i < actual.size(); i++)
    {
        if (actual[i] != supplied[i])
        {
            areSame = false;
            break;
        }
    }

    return areSame;
}

size_t TypeChecker::ResolveNativeFunctions(std::string &name, std::vector<TypeData> &argtypes)
{
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < funcs.size(); i++)
    {
        if (NativeFunctions.find(funcs[i].name) == NativeFunctions.end())
            break;

        if (funcs[i].name == name && MatchNativeArguments(funcs[i].argtypes, argtypes))
            index = i;
    }
    return index;
}

size_t TypeChecker::ResolveFunction(std::string &name, std::vector<TypeData> &argtypes)
{
    size_t isNative = TypeChecker::ResolveNativeFunctions(name, argtypes);
    if (isNative != SIZE_MAX)
        return isNative;

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
    return SIZE_MAX;
}

bool TypeChecker::MatchInitialiserToStruct(const std::vector<TypeData> &member, const std::vector<TypeData> &init)
{
    if (member.size() != init.size())
        return false;

    for (size_t i = 0; i < member.size(); i++)
    {
        if (!CanAssign(member[i], init[i]))
            return false;
    }

    return true;
}

size_t TypeChecker::FindStruct(const TypeData &structT)
{
    for (size_t i = 0; i < structTypes.size(); i++)
    {
        if (structTypes[i].type == structT)
            return i;
    }
    return SIZE_MAX;
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

    return {false, 0};
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
    return {false, 0};
}

TypeData TypeChecker::TypeOfAssign(Assign *a)
{
    VarReference *targetAsVr = dynamic_cast<VarReference *>(a->target.get());

    TypeData valType = a->val->Type(*this);

    if (targetAsVr != nullptr)
    {
        size_t varIndex = ResolveVariable(targetAsVr->name, a->Loc());
        TypeData varType = vars[varIndex].type;

        if (!CanAssign(varType, valType))
            TypeError(a->Loc(), "Cannot assign " + ToString(valType) + " to variable of type " + ToString(varType));

        targetAsVr->isArray = varType.isArray;
        targetAsVr->t = varType;
        a->val->t = varType;
        a->t = varType;

        return varType;
    }

    TypeData targetType = a->target->Type(*this);
    if (!CanAssign(targetType, valType))
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
    {
        try
        {
            argtypes.push_back(e->Type(*this));
        }
        catch (const std::exception &e)
        {
            hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }

    size_t index = ResolveFunction(fc->name, argtypes);

    if (index == SIZE_MAX)
        TypeError(fc->Loc(), "Function: '" + fc->name + "' has not been defined yet");

    if (index > UINT8_MAX)
        TypeError(fc->Loc(), "Cannot have more than " + std::to_string(UINT8_MAX) + " functions");

    fc->t = funcs[index].ret;
    return funcs[index].ret;
}

TypeData TypeChecker::TypeOfArrayIndex(ArrayIndex *ai)
{
    TypeData nameT = ai->name->Type(*this);

    TypeData stringT = {false, 4};

    if (!nameT.isArray && nameT != stringT)
        TypeError(ai->Loc(), "Cannot index into object of type " + ToString(nameT));

    if (nameT.isArray)
    {
        ai->t = nameT;
        ai->t.isArray = false;
    }

    if(nameT == stringT)
        ai->t = {false, 5};

    return ai->t;
}

TypeData TypeChecker::TypeOfBracedInitialiser(BracedInitialiser *bi)
{
    if (bi->size == 0)
        return {false, 0};

    // getting types of braced initialiser
    std::vector<TypeData> types;
    for (auto &e : bi->init)
    {
        try
        {
            types.push_back(e->Type(*this));
        }
        catch (const std::exception &e)
        {
            hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }

    TypeData voidType = {false, 0};
    // if not declared with a type checks to see if it is an array
    if (bi->t == voidType)
    {
        TypeData first = bi->init[0]->Type(*this);
        for (size_t i = 0; i < bi->init.size(); i++)
        {
            if (!CanAssign(first, bi->init[i]->Type(*this)))
                TypeError(bi->init[i]->Loc(), "Types of expression in a braced initialiser must be assignable to each other");
        }
        bi->t = first;
        bi->t.isArray = true;
        return bi->t;
    }
    // otherwise checks to see if elements match the required type
    else
    {
        TypeData toComapre = bi->t;
        if (bi->t.isArray)
            toComapre.isArray = false;
        else
        {
            if (bi->t.type < NUM_DEF_TYPES)
                TypeError(bi->Loc(), "Cannot declare a braced initialiser with " + ToString(bi->t) + " type");

            for (size_t i = 0; i < structTypes.size(); i++)
            {
                if (structTypes[i].type == bi->t && MatchInitialiserToStruct(structTypes[i].members, types))
                    return structTypes[i].type;
            }
        }

        for (size_t i = 0; i < bi->init.size(); i++)
        {
            if (!CanAssign(toComapre, bi->init[i]->Type(*this)))
                TypeError(bi->init[i]->Loc(), "Type of expression " + ToString(bi->init[i]->t) + " in a braced initialiser must be assignable to the type specified at its beginning " + ToString(bi->t));
        }
        return bi->t;
    }
}

TypeData TypeChecker::TypeOfDynamicAllocArray(DynamicAllocArray *da)
{
    TypeData sizeType = da->size->Type(*this);
    TypeData intType = {false, 1};

    if (sizeType != intType)
        TypeError(da->Loc(), "Size of dynamically allocated array must have type int");

    return da->t;
}

TypeData TypeChecker::TypeOfFieldAccess(FieldAccess *fa)
{
    TypeData accessorType = fa->accessor->Type(*this);

    size_t structIndex = FindStruct(accessorType);

    if (structIndex == SIZE_MAX)
        TypeError(fa->Loc(), "Type " + ToString(accessorType) + " cannot be accesed into");

    StructID s = structTypes[structIndex];

    depth++;

    for (const auto &kv : s.nameTypes)
        vars.push_back({kv.second, kv.first, depth});

    try
    {
        TypeData accesseeType = fa->accessee->Type(*this);
        fa->t = accesseeType;
        CleanUpVariables();
        return accesseeType;
    }
    catch (const std::exception &e)
    {
        hadError = true;
        std::cerr << "Invalid struct field access:" << std::endl
                  << e.what() << std::endl;
    }
    return {false, 0};
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

        if (!CanAssign(valType, varType))
            TypeError(dv->Loc(), "Cannot assign value of type: " + ToString(valType) + " to variable: '" + dv->name + "' of type: " + ToString(varType));

        dv->value->t = varType;
    }
    return {false, 0};
}

TypeData TypeChecker::TypeOfBlock(Block *b)
{
    depth++;

    if (depth == UINT8_MAX)
        TypeError(b->Loc(), "Exceeded maximum number of nested blocks: " + std::to_string(UINT8_MAX));

    for (std::shared_ptr<Stmt> &s : b->stmts)
    {
        try
        {
            s->Type(*this);
        }
        catch (const std::exception &e)
        {
            hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }
    // CleanUpVariables();
    depth--;
    return {false, 0};
}

TypeData TypeChecker::TypeOfIfStmt(IfStmt *i)
{
    if (!IsTruthy(i->cond->Type(*this)))
        TypeError(i->Loc(), "Condition of and if statement must be 'turthy'");

    i->thenBranch->Type(*this);

    if (i->elseBranch != nullptr)
        i->elseBranch->Type(*this);

    return {false, 0};
}

TypeData TypeChecker::TypeOfWhileStmt(WhileStmt *ws)
{
    if (!IsTruthy(ws->cond->Type(*this)))
        TypeError(ws->Loc(), "Condition of a while statment must be 'truthy'");

    ws->body->Type(*this);
    return {false, 0};
}

TypeData TypeChecker::TypeOfFuncDecl(FuncDecl *fd)
{
    if (ResolveFunction(fd->name, fd->argtypes) != SIZE_MAX)
        TypeError(fd->Loc(), "Function '" + fd->name + "' has already been defined");

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
    {
        try
        {
            s->Type(*this);
        }
        catch (const std::exception &e)
        {
            hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }

    isInFunc = false;
    CleanUpVariables();
    funcVarBegin = 0;
    depth--;
    return {false, 0};
}

TypeData TypeChecker::TypeOfReturn(Return *r)
{
    if (depth == 0)
        TypeError(r->Loc(), "Cannot return from outside of a function");
    if (r->retVal == nullptr)
        return {false, 0};
    return r->retVal->Type(*this);
}

TypeData TypeChecker::TypeOfStructDecl(StructDecl *sd)
{
    if (sd->decls.size() > UINT8_MAX)
        TypeError(sd->Loc(), "Structs can only have a maximum of " + std::to_string(UINT8_MAX) + " members");
    depth++;

    StructID s;
    s.type = GetTypeNameMap()[sd->name];
    for (auto &d : sd->decls)
    {
        try
        {
            d->Type(*this);
            DeclaredVar *asDV = dynamic_cast<DeclaredVar *>(d.get());

            if (asDV == nullptr)
                TypeError(d->Loc(), "The body of struct declarations can only consist of variable declarations");

            s.members.push_back(asDV->t);
            s.nameTypes[asDV->name] = asDV->t;
        }
        catch (std::exception &e)
        {
            hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }
    structTypes.push_back(s);

    CleanUpVariables();
    depth--;
    return {false, 0};
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

TypeData BracedInitialiser::Type(TypeChecker &t)
{
    return t.TypeOfBracedInitialiser(this);
}

TypeData DynamicAllocArray::Type(TypeChecker &t)
{
    return t.TypeOfDynamicAllocArray(this);
}

TypeData FieldAccess::Type(TypeChecker &t)
{
    return t.TypeOfFieldAccess(this);
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

TypeData StructDecl::Type(TypeChecker &t)
{
    return t.TypeOfStructDecl(this);
}