#include "nodecompiler.h"

/*
    TODO
        Compile uninitialised variables
*/

//-----------------EXPRESSIONS---------------------//

#define GET_TYPED_BINARY_OP(l, op, r, ret)   \
    do                                       \
    {                                        \
        if (l.type == 1 && r.type == 1)      \
            ret = Opcode::I_##op;            \
        else if (l.type == 1 && r.type == 2) \
            ret = Opcode::ID_##op;           \
        else if (l.type == 2 && r.type == 1) \
            ret = Opcode::DI_##op;           \
        else                                 \
            ret = Opcode::D_##op;            \
    } while (false)

#define GET_TYPED_UNARY_OP(op, r, ret) \
    do                                 \
    {                                  \
        if (r.type == 1)               \
            ret = Opcode::I_##op;      \
        else if (r.type == 2)          \
            ret = Opcode::D_##op;      \
    } while (false)

Opcode TokenToOpcode(TypeData l, TokenID t, TypeData r, bool isUnary)
{
    Opcode o;
    if (t == TokenID::PLUS)
    {
        if (l.type == 4 && r.type == 4)
            return Opcode::S_ADD;

        GET_TYPED_BINARY_OP(l, ADD, r, o);
        return o;
    }
    else if (t == TokenID::MINUS)
    {
        if (isUnary)
            GET_TYPED_UNARY_OP(SUB, r, o);
        else
            GET_TYPED_BINARY_OP(l, SUB, r, o);
        return o;
    }
    else if (t == TokenID::STAR)
    {
        GET_TYPED_BINARY_OP(l, MUL, r, o);
        return o;
    }
    else if (t == TokenID::SLASH)
    {
        GET_TYPED_BINARY_OP(l, DIV, r, o);
        return o;
    }
    else if (t == TokenID::GT)
    {
        GET_TYPED_BINARY_OP(l, GT, r, o);
        return o;
    }
    else if (t == TokenID::LT)
    {
        GET_TYPED_BINARY_OP(l, LT, r, o);
        return o;
    }
    else if (t == TokenID::GEQ)
    {
        GET_TYPED_BINARY_OP(l, GEQ, r, o);
        return o;
    }
    else if (t == TokenID::LEQ)
    {
        GET_TYPED_BINARY_OP(l, LEQ, r, o);
        return o;
    }
    else if (t == TokenID::EQ_EQ)
    {
        if (l.type == 3 && r.type == 3)
            return Opcode::B_EQ_EQ;
        else if (l.type == 6 || r.type == 6)
            return Opcode::N_EQ_EQ;
        GET_TYPED_BINARY_OP(l, EQ_EQ, r, o);
        return o;
    }
    else if (t == TokenID::BANG_EQ)
    {
        if (l.type == 3 && r.type == 3)
            return Opcode::B_BANG_EQ;
        else if (l.type == 6 || r.type == 6)
            return Opcode::N_BANG_EQ;
        GET_TYPED_BINARY_OP(l, BANG_EQ, r, o);
        return o;
    }
    else if (t == TokenID::AND_AND)
        return Opcode::B_AND_AND;
    else if (t == TokenID::OR_OR)
        return Opcode::B_OR_OR;
    else if (t == TokenID::BANG)
    {
        return Opcode::BANG;
    }
    else
        return Opcode::NONE;
}

bool IsTruthy(const TypeData &td)
{
    return td == INT_TYPE || td == BOOL_TYPE;
}

TypeData NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    TypeData type = l->t;
    std::string literal = l->Loc().literal;
    Function *cur = c.cur;

    if (type == INT_TYPE)
    {
        cur->ints.push_back(std::stoi(literal));
        if (cur->ints.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of int constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_INT, static_cast<oprand_t>(cur->ints.size() - 1)});
    }
    else if (type == DOUBLE_TYPE)
    {
        cur->doubles.push_back(std::stod(literal));
        if (cur->doubles.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of double constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_DOUBLE, static_cast<oprand_t>(cur->doubles.size() - 1)});
    }
    else if (type == BOOL_TYPE)
    {
        cur->bools.push_back(literal == "true" ? true : false);
        if (cur->bools.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of bool constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_BOOL, static_cast<oprand_t>(cur->bools.size() - 1)});
    }
    else if (type == STRING_TYPE)
    {
        cur->strings.push_back(literal);
        if (cur->strings.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of string constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_STRING, static_cast<oprand_t>(cur->strings.size() - 1)});
    }
    else if (type == CHAR_TYPE)
    {
        cur->chars.push_back(literal[0]);
        if (cur->chars.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of char constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_CHAR, static_cast<oprand_t>(cur->chars.size() - 1)});
    }
    else
        c.TypeError(l->Loc(), "Cannot have a literal of type " + ToString(l->t));
    return l->t;
}

TypeData NodeCompiler::CompileUnary(Unary *u, Compiler &c)
{
    TypeData right = u->right->NodeCompile(c);

    if (!CheckOperatorUse(VOID_TYPE, u->op.type, right))
        c.TypeError(u->op, "Cannot use oprand of type " + ToString(right) + " with operator " + u->op.literal);

    TypeData result = OperatorResult(VOID_TYPE, u->op.type, right);
    c.AddCode({TokenToOpcode(VOID_TYPE, u->op.type, right, true), 1});
    return result;
}

TypeData NodeCompiler::CompileBinary(Binary *b, Compiler &c)
{
    TypeData left = b->left->NodeCompile(c);
    TypeData right = b->right->NodeCompile(c);

    if (!CheckOperatorUse(left, b->op.type, right))
        c.TypeError(b->op, "Cannot use operands of type " + ToString(left) + " and " + ToString(right) + "with operator " + b->op.literal);

    TypeData result = OperatorResult(left, b->op.type, right);
    c.AddCode({TokenToOpcode(left, b->op.type, right, false), 0});
    return result;
}

TypeData NodeCompiler::CompileAssign(Assign *a, Compiler &c)
{
    /*
    Valid assignment targets
    1) VarReference
    2) ArrayIndex
    3) FieldAccess
    */

    TypeData value = a->val->NodeCompile(c);

    VarReference *targetAsVR = dynamic_cast<VarReference *>(a->target.get());
    if (targetAsVR != nullptr)
    {
        VarID *vid = c.Symbols.GetVar(targetAsVR->name);
        if (vid == nullptr)
            c.SymbolError(targetAsVR->Loc(), "Reference to variable '" + targetAsVR->name + "' is not defined");

        if (!c.Symbols.CanAssign(vid->type, value))
            c.TypeError(a->Loc(), "Cannot assign object of type " + ToString(value) + " to variable of type " + ToString(vid->type));

        size_t varStackLoc = c.Symbols.GetVarStackLoc(targetAsVR->name);
        if (varStackLoc > MAX_OPRAND)
            c.CompileError(targetAsVR->Loc(), "Too many variables");

        if (vid->depth == 0)
            c.AddCode({Opcode::VAR_A_GLOBAL, static_cast<oprand_t>(varStackLoc)});
        else
            c.AddCode({Opcode::VAR_A, static_cast<oprand_t>(varStackLoc)});

        return vid->type;
    }

    ArrayIndex *targetAsAI = dynamic_cast<ArrayIndex *>(a->target.get());
    if (targetAsAI != nullptr)
    {
        TypeData name = targetAsAI->name->NodeCompile(c);
        if (!name.isArray && name != STRING_TYPE)
            c.TypeError(targetAsAI->Loc(), "Cannot index type " + ToString(name));

        TypeData index = targetAsAI->index->NodeCompile(c);
        if (index != INT_TYPE)
            c.TypeError(targetAsAI->Loc(), "Index into array/string must be of type int not " + ToString(index));

        if (name.isArray)
        {
            c.AddCode({Opcode::ARR_SET, 0});
            TypeData result = name;
            result.isArray--;
            return result;
        }
        else
        {
            c.AddCode({Opcode::STRING_SET, 0});
            return CHAR_TYPE;
        }
    }

    FieldAccess *targetAsFA = dynamic_cast<FieldAccess *>(a->target.get());
    if (targetAsFA != nullptr)
    {
        TypeData accessee = targetAsFA->accessor->NodeCompile(c);
        StructID *sid = c.Symbols.GetStruct(accessee);

        VarReference *vrAccessee = dynamic_cast<VarReference *>(targetAsFA->accessee.get());
        if (vrAccessee == nullptr)
            c.CompileError(targetAsFA->accessee->Loc(), "Invalid struct accessee");

        size_t strctMem = SIZE_MAX;
        for (size_t i = 0; i < sid->memberNames.size(); i++)
        {
            if (sid->memberNames[i] == vrAccessee->name)
            {
                strctMem = i;
                break;
            }
        }

        if (strctMem == SIZE_MAX)
            c.CompileError(targetAsFA->accessee->Loc(), "Struct " + sid->name + " has no member " + vrAccessee->name);

        c.AddCode({Opcode::STRUCT_MEMBER_SET, static_cast<oprand_t>(strctMem)});
    }

    c.CompileError(a->target->Loc(), "Invalid assignment target");
    return VOID_TYPE;
}

TypeData NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    VarID *vid = c.Symbols.GetVar(vr->name);
    if (vid == nullptr)
        c.SymbolError(vr->Loc(), "Variable '" + vr->name + "' has not been defined");

    size_t stackLoc = c.Symbols.GetVarStackLoc(vr->name);
    if (stackLoc > MAX_OPRAND)
        c.CompileError(vr->Loc(), "Too many variables, maximum number is " + std::to_string(MAX_OPRAND));

    if (vid->depth > 0)
        c.AddCode({Opcode::GET_V, static_cast<oprand_t>(stackLoc)});
    else
        c.AddCode({Opcode::GET_V_GLOBAL, static_cast<oprand_t>(stackLoc)});
    return vid->type;
}

TypeData NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{
    std::vector<TypeData> args;
    for (auto &e : fc->args)
        args.push_back(e->NodeCompile(c));

    FuncID *fid = c.Symbols.GetFunc(fc->name, fc->templates, args);
    if (fid == nullptr)
    {
        std::ostringstream out;
        out << fc->name;

        if (fc->templates.size() > 0)
        {
            out << "<|";
            for (size_t i = 0; i < fc->templates.size(); i++)
            {
                out << ToString(fc->templates[i]);
                if (i != fc->templates.size() - 1)
                    out << ", ";
            }
            out << "|>";
        }

        out << "(";
        if (args.size() > 0)
        {
            for (size_t i = 0; i < args.size(); i++)
            {
                out << ToString(args[i]);
                if (i != args.size() - 1)
                    out << ", ";
            }
        }
        out << ")";
        c.SymbolError(fc->Loc(), "Function '" + out.str() + "' has not been defined yet");
    }

    switch (fid->kind)
    {
    case FunctionType::USER_DEFINED:
    {
        size_t funcNum = c.Symbols.GetUDFuncNum(fid);
        if (funcNum > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many functions, maximum number is " + std::to_string(MAX_OPRAND));
        c.AddCode({Opcode::CALL_F, static_cast<oprand_t>(funcNum + 1)});
        break;
    }
    case FunctionType::USER_DEFINED_TEMPLATE:
    {
        break;
    }
    case FunctionType::LIBRARY:
    {
        size_t funcNum = c.Symbols.GetCLibFuncNum(fid);
        if (funcNum > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many C library functions, maximum number is " + std::to_string(MAX_OPRAND));
        c.AddCode({Opcode::NATIVE_CALL, static_cast<oprand_t>(funcNum + 1)});
        break;
    }
    case FunctionType::NATIVE:
    {
        size_t funcNum = c.Symbols.GetNativeFuncNum(fid);
        if (funcNum > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many C library functions, maximum number is " + std::to_string(MAX_OPRAND));
        if (fc->name != "Print")
            c.AddCode({Opcode::NATIVE_CALL, static_cast<oprand_t>(funcNum + 1)});
        else
        {
            if (fc->args.size() > MAX_OPRAND)
                c.CompileError(fc->Loc(), "Maximum number of arguments to Print is " + std::to_string(MAX_OPRAND));
            c.AddCode({Opcode::PRINT, static_cast<oprand_t>(fc->args.size())});
        }
        break;
    }
    }
    return fid->ret;
}

TypeData NodeCompiler::CompileArrayIndex(ArrayIndex *ai, Compiler &c)
{
    TypeData name = ai->name->NodeCompile(c);

    if (!name.isArray && name != STRING_TYPE)
        c.TypeError(ai->Loc(), "Cannot index into object of type " + ToString(name));

    TypeData index = ai->index->NodeCompile(c);
    if (index != INT_TYPE)
        c.TypeError(ai->index->Loc(), "Indices can only be of type int not " + ToString(index));

    if (name.isArray)
    {
        c.AddCode({Opcode::ARR_INDEX, 0});
        name.isArray--;
        return name;
    }
    else
    {
        c.AddCode({Opcode::STRING_INDEX, 0});
        return CHAR_TYPE;
    }
}

TypeData NodeCompiler::CompileBracedInitialiser(BracedInitialiser *bi, Compiler &c)
{
    if (bi->size > MAX_OPRAND)
        c.CompileError(bi->Loc(), "Braced initialisers can only have " + std::to_string(MAX_OPRAND) + " elements");

    TypeData biType = bi->t;

    c.AddCode({Opcode::ARR_ALLOC, static_cast<oprand_t>(bi->size)});
    std::pair<size_t, size_t> allocTypeLoc = c.LastAddedCodeLoc();

    std::vector<TypeData> args;
    for (auto &e : bi->init)
        args.push_back(e->NodeCompile(c));

    if (biType.isArray)
    {
        TypeData target = biType;
        target.isArray--;

        for (size_t i = 0; i < bi->init.size(); i++)
        {
            if (!c.Symbols.CanAssign(target, args[i]))
                c.TypeError(bi->init[i]->Loc(), "Cannot object of type " + ToString(args[i]) + " to the required type of the array specified at the beginning of the braced initialiser");
        }

        c.AddCode({Opcode::ARR_D, static_cast<oprand_t>(bi->size)});
        return biType;
    }
    else
    {
        StructID *sid = c.Symbols.GetStruct(biType);
        if (sid == nullptr)
            c.TypeError(bi->Loc(), "Invalid struct name in braced initialiser");
        if (sid->memTypes.size() != args.size())
            c.TypeError(bi->Loc(), "Number of arguments in braced initialiser is not equal to the number of arguments in struct declaration");

        c.ModifyOpcodeAt(allocTypeLoc, Opcode::STRUCT_ALLOC);
        for (size_t i = 0; i < bi->size; i++)
        {
            if (!c.Symbols.CanAssign(sid->memTypes[i], args[i]))
                c.TypeError(bi->init[i]->Loc(), "Object in struct braced initialiser of type " + ToString(args[i]) + " cannot be assigned to object of type " + ToString(sid->memTypes[i]));
        }
        c.AddCode({Opcode::STRUCT_D, static_cast<oprand_t>(bi->size)});
        return sid->type;
    }
}

TypeData NodeCompiler::CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c)
{
    if (!da->t.isArray)
        c.TypeError(da->Loc(), "Must have array type");
    TypeData size = da->size->NodeCompile(c);
    if (size != INT_TYPE)
        c.TypeError(da->Loc(), "Size of dynamically allocated array must have type int not " + ToString(size));

    c.AddCode({Opcode::ARR_ALLOC, 0});
    return da->t;
}

TypeData NodeCompiler::CompileFieldAccess(FieldAccess *fa, Compiler &c)
{
    TypeData accessor = fa->accessor->NodeCompile(c);

    StructID *sid = c.Symbols.GetStruct(accessor);
    if (sid == nullptr)
        c.TypeError(fa->Loc(), "Type " + ToString(accessor) + " cannot be accessed into");

    VarReference *vAccessee = dynamic_cast<VarReference *>(fa->accessee.get());
    if (vAccessee == nullptr)
        c.TypeError(fa->accessee->Loc(), "Only an identifier can be used as a field accessee");

    // index of accessee in the underlying array
    size_t index = SIZE_MAX;
    for (size_t i = 0; i < sid->memberNames.size(); i++)
    {
        if (sid->memberNames[i] == vAccessee->name)
        {
            index = i;
            break;
        }
    }

    if (index == SIZE_MAX)
        c.SymbolError(fa->accessee->Loc(), "Struct " + sid->name + " does not have member " + vAccessee->name);

    c.AddCode({Opcode::STRUCT_MEMBER, static_cast<oprand_t>(index)});

    return sid->nameTypes[vAccessee->name];
}

TypeData NodeCompiler::CompileTypeCast(TypeCast *tc, Compiler &c)
{
    TypeData old = tc->arg->NodeCompile(c);
    TypeData nw = tc->type;

    bool isDownCast = c.Symbols.CanAssign(nw, old);
    bool isUpCast = c.Symbols.CanAssign(old, nw);

    if (!isDownCast && !isUpCast)
        c.TypeError(tc->Loc(), "Cannot cast " + ToString(old) + " to " + ToString(nw));

    c.AddCode({Opcode::CAST, tc->type.type});
    return tc->type;
}

//------------------STATEMENTS---------------------//

void NodeCompiler::CompileExprStmt(ExprStmt *es, Compiler &c)
{
    TypeData exp = es->exp->NodeCompile(c);
    if (exp != VOID_TYPE)
        c.AddCode({Opcode::POP, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    if (c.Symbols.vars.size() > MAX_OPRAND)
        c.CompileError(dv->Loc(), "Maximum number of variables is " + std::to_string(MAX_OPRAND));

    VarID *vid = c.Symbols.GetVar(dv->name);
    if (vid != nullptr && vid->depth == c.Symbols.depth)
        c.SymbolError(dv->Loc(), "Variable '" + ToString(dv->t) + " " + dv->name + "' has already been defined in this scope");

    if (dv->value != nullptr)
    {
        TypeData val = dv->value->NodeCompile(c);
        if (!c.Symbols.CanAssign(dv->t, val))
            c.TypeError(dv->Loc(), "Cannot initialise variable '" + dv->name + "' of type " + ToString(dv->t) + " with expression of type " + ToString(val));

        if (c.Symbols.depth == 0)
            c.AddCode({Opcode::VAR_D_GLOBAL, 0});
    }
    else
    {
        if (c.Symbols.depth == 0)
            c.SymbolError(dv->Loc(), "Global variable must be initialised");
    }
    c.Symbols.AddVar(dv->t, dv->name);
}

void NodeCompiler::CompileBlock(Block *b, Compiler &c)
{
    c.Symbols.depth++;
    for (auto &stmt : b->stmts)
        stmt->NodeCompile(c);

    c.ClearCurrentDepthWithPOPInst();
    c.Symbols.depth--;
}

void NodeCompiler::CompileIfStmt(IfStmt *i, Compiler &c)
{
    TypeData cond = i->cond->NodeCompile(c);
    if (!IsTruthy(cond))
        c.TypeError(i->cond->Loc(), "Condition of if statement must be convertible to bool");

    c.AddCode({Opcode::GOTO_LABEL_IF_FALSE, 0});
    std::pair<size_t, size_t> notTrue = c.LastAddedCodeLoc();

    c.AddCode({Opcode::GOTO_LABEL, 0});
    std::pair<size_t, size_t> isTrue = c.LastAddedCodeLoc();

    c.AddRoutine();
    i->thenBranch->NodeCompile(c);
    size_t thenRoutine = c.GetCurRoutineIndex();

    if (thenRoutine > MAX_OPRAND)
        c.CompileError(i->thenBranch->Loc(), "Too many routines");
    // isTrue->op = static_cast<oprand_t>(thenRoutine);
    c.ModifyOprandAt(isTrue, static_cast<oprand_t>(thenRoutine));

    c.AddCode({Opcode::GOTO_LABEL, 0});
    std::pair<size_t, size_t> thenReturn = c.LastAddedCodeLoc();

    if (i->elseBranch == nullptr)
    {
        c.AddRoutine();
        size_t newRoutine = c.GetCurRoutineIndex();
        if (newRoutine > MAX_OPRAND)
            c.CompileError(i->thenBranch->Loc(), "Too many routines");
        c.ModifyOprandAt(thenReturn, static_cast<oprand_t>(newRoutine));
        c.ModifyOprandAt(notTrue, static_cast<oprand_t>(newRoutine));
    }
    else
    {
        c.AddRoutine();
        i->elseBranch->NodeCompile(c);

        size_t elseIndex = c.GetCurRoutineIndex();
        if (elseIndex > MAX_OPRAND)
            c.CompileError(i->elseBranch->Loc(), "Too many routines");
        c.ModifyOprandAt(notTrue, static_cast<oprand_t>(elseIndex));

        c.AddCode({Opcode::GOTO_LABEL, 0});
        std::pair<size_t, size_t> elseReturn = c.LastAddedCodeLoc();

        c.AddRoutine();
        size_t newRoutine = c.GetCurRoutineIndex();
        if (newRoutine > MAX_OPRAND)
            c.CompileError(i->elseBranch->Loc(), "Too many routines");
        c.ModifyOprandAt(thenReturn, static_cast<oprand_t>(newRoutine));
        c.ModifyOprandAt(elseReturn, static_cast<oprand_t>(newRoutine));
    }
}

void NodeCompiler::CompileWhileStmt(WhileStmt *ws, Compiler &c)
{
    c.AddCode({Opcode::GOTO_LABEL, 0});
    std::pair<size_t, size_t> enter = c.LastAddedCodeLoc();

    c.AddRoutine();
    size_t loopRoutine = c.GetCurRoutineIndex();
    if (loopRoutine > MAX_OPRAND)
        c.CompileError(ws->Loc(), "Too many routines");

    c.ModifyOprandAt(enter, static_cast<oprand_t>(loopRoutine));

    TypeData cond = ws->cond->NodeCompile(c);
    if (!IsTruthy(cond))
        c.TypeError(ws->cond->Loc(), "Condition of a loop must be convertible to bool");

    c.AddCode({Opcode::GOTO_LABEL_IF_FALSE, 0});
    std::pair<size_t, size_t> notTrue = c.LastAddedCodeLoc();

    ws->body->NodeCompile(c);
    c.AddCode({Opcode::GOTO_LABEL, static_cast<oprand_t>(loopRoutine)});

    c.AddRoutine();
    size_t newRoutine = c.GetCurRoutineIndex();
    if (newRoutine > MAX_OPRAND)
        c.CompileError(ws->Loc(), "Too many routines");

    c.ModifyOprandAt(notTrue, static_cast<oprand_t>(newRoutine));
}

void NodeCompiler::CompileFuncDecl(FuncDecl *fd, Compiler &c)
{
    // TODO - Check function is not already defined
    c.AddFunction();

    c.cur->arity = fd->argtypes.size();

    std::vector<TypeData> templates;
    for (auto &t : fd->templates)
        templates.push_back(t.first);

    c.Symbols.AddFunc(FuncID(fd->ret, fd->name, templates, fd->argtypes, FunctionType::USER_DEFINED, c.parseIndex));

    if (fd->templates.size() > 0)
        return;

    c.Symbols.depth++;
    for (size_t i = 0; i < fd->argtypes.size(); i++)
        c.Symbols.AddVar(fd->argtypes[i], fd->paramIdentifiers[i]);

    for (auto &stmt : fd->body)
        stmt->NodeCompile(c);

    c.ClearCurrentDepthWithPOPInst();
    c.Symbols.depth--;
    c.cur = &c.Functions[0];
}

void NodeCompiler::CompileReturn(Return *r, Compiler &c)
{
    if (r->retVal == nullptr)
        c.AddCode({Opcode::RETURN_VOID, 0});
    else
    {
        // TODO - check return type can be assigned to one
        // specified at function declaration
        r->retVal->NodeCompile(c);
        c.AddCode({Opcode::RETURN, 0});
    }
}

void NodeCompiler::CompileStructDecl(StructDecl *sd, Compiler &c)
{
    std::string name = sd->name;
    TypeData type = GetTypeNameMap()[name];

    if (type.type < NUM_DEF_TYPES)
        c.SymbolError(sd->Loc(), "Invalid struct name");

    TypeData parent = sd->parent;
    std::vector<std::string> memberNames;
    std::vector<TypeData> memTypes;
    std::vector<std::shared_ptr<Expr>> init;
    std::unordered_map<std::string, TypeData> nameTypes;

    if (parent != VOID_TYPE)
    {
        if (parent.isArray)
            c.TypeError(sd->Loc(), "Parent of a struct cannot be array");
        StructID *sidParent = c.Symbols.GetStruct(parent);

        if (sidParent == nullptr)
            c.TypeError(sd->Loc(), "Invalid parent struct name");

        for (const std::string &name : sidParent->memberNames)
            memberNames.push_back(name);

        for (const TypeData &type : sidParent->memTypes)
            memTypes.push_back(type);

        for (const auto kv : sidParent->nameTypes)
            nameTypes[kv.first] = kv.second;
    }

    for (auto &d : sd->decls)
    {
        DeclaredVar *asDV = dynamic_cast<DeclaredVar *>(d.get());
        if (asDV == nullptr)
            c.TypeError(d->Loc(), "The body of struct declarations can only consist of variable declarations");

        if (asDV->value != nullptr)
            c.CompileError(asDV->value->Loc(), "Variable declarations inside struct declarations cannot have values");

        memberNames.push_back(asDV->name);
        memTypes.push_back(asDV->t);
        nameTypes[asDV->name] = asDV->t;
    }

    c.Symbols.AddStruct(StructID(name, type, parent, memberNames, memTypes, nameTypes));
}

void NodeCompiler::CompileImportStmt(ImportStmt *is, Compiler &c)
{
    std::vector<std::string> libraryFuncs;
    assert(is->libraries.size() > 0);
    for (const auto library : is->libraries)
    {
        libraryFuncs = c.Symbols.GetLibraryFunctionNames(library);
        for (auto &lf : libraryFuncs)
        {
            FuncID func = c.Symbols.ParseLibraryFunction(lf);
            c.Symbols.AddCLibFunc(func);

            if (c.Symbols.clibFunctions.size() > MAX_OPRAND)
                c.CompileError(is->Loc(), "Cannot import more than " + std::to_string(MAX_OPRAND) + " library functions in total");

            c.libfuncs.push_back(LibraryFunctionDef(func.name, library, func.argtypes.size()));
        }
    }
}

void NodeCompiler::CompileBreak(Break *b, Compiler &c)
{
    if (c.breakIndices.size() == 0)
        c.CompileError(b->Loc(), "Break statement cannot occur outside of a loop");

    std::vector<size_t> *curLoopBreaks = &c.breakIndices.top();
    size_t breakLoc = c.CodeSize();

    curLoopBreaks->push_back(breakLoc);
    c.AddCode({Opcode::SET_IP, 0});
}

void NodeCompiler::CompileThrow(Throw *t, Compiler &c)
{
    t->exp->NodeCompile(c);
    c.AddCode({Opcode::THROW, 0});
}

void NodeCompiler::CompilerTryCatch(TryCatch *tc, Compiler &c)
{
    ThrowInfo ti;
    ti.func = c.cur - &c.Functions[0];

    TypeData catchType = tc->catchVar.first;
    std::string catchVarName = tc->catchVar.second;

    ti.type = catchType.type;
    ti.isArray = catchType.isArray;

    size_t throwInfoSize = c.throwStack.size();
    if (throwInfoSize > MAX_OPRAND)
        c.CompileError(tc->Loc(), "Too many try-catch blocks, maximum number is " + std::to_string(MAX_OPRAND));

    c.AddCode({Opcode::PUSH_THROW_INFO, static_cast<oprand_t>(throwInfoSize)});
    tc->tryClause->NodeCompile(c);

    size_t sIndex = c.CodeSize();
    if (sIndex > MAX_OPRAND)
        c.CompileError(tc->tryClause->Loc(), "Too much code generated from 'try' clause");

    ti.index = static_cast<oprand_t>(sIndex);
    c.throwStack.push_back(ti);
    c.Symbols.AddVar(catchType, catchVarName);
    tc->catchClause->NodeCompile(c);
}

//-----------------EXPRESSIONS---------------------//

TypeData Literal::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileLiteral(this, c);
}

TypeData Unary::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileUnary(this, c);
}

TypeData Binary::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileBinary(this, c);
}

TypeData Assign::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileAssign(this, c);
}

TypeData VarReference::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileVarReference(this, c);
}

TypeData FunctionCall::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileFunctionCall(this, c);
}

TypeData ArrayIndex::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileArrayIndex(this, c);
}

TypeData BracedInitialiser::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileBracedInitialiser(this, c);
}

TypeData DynamicAllocArray::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileDynamicAllocArray(this, c);
}

TypeData FieldAccess::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileFieldAccess(this, c);
}

TypeData TypeCast::NodeCompile(Compiler &c)
{
    return NodeCompiler::CompileTypeCast(this, c);
}

//------------------STATEMENTS---------------------//

void ExprStmt::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileExprStmt(this, c);
}

void DeclaredVar::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileDeclaredVar(this, c);
}

void Block::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileBlock(this, c);
}

void IfStmt::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileIfStmt(this, c);
}

void WhileStmt::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileWhileStmt(this, c);
}

void FuncDecl::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileFuncDecl(this, c);
}

void Return::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileReturn(this, c);
}

void StructDecl::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileStructDecl(this, c);
}

void ImportStmt::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileImportStmt(this, c);
}

void Break::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileBreak(this, c);
}

void Throw::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileThrow(this, c);
}

void TryCatch::NodeCompile(Compiler &c)
{
    NodeCompiler::CompilerTryCatch(this, c);
}