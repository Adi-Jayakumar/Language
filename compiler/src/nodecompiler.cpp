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

inline bool IsTruthy(const TypeData &td)
{
    return td == INT_TYPE || td == BOOL_TYPE;
}

#define GET(x)                                                                 \
    inline Opcode GetGETInstruction(const TypeData &type, const bool isGlobal) \
    {                                                                          \
                                                                               \
        if (type.isArray && isGlobal)                                          \
            return Opcode::GET_ARRAY_GLOBAL;                                   \
        else if (type.isArray && !isGlobal)                                    \
            return Opcode::GET_ARRAY;                                          \
        else if (type.type > (NUM_DEF_TYPES - 1) && isGlobal)                  \
            return Opcode::GET_STRUCT_GLOBAL;                                  \
        else if (type.type > (NUM_DEF_TYPES - 1) && !isGlobal)                 \
            return Opcode::GET_STRUCT;                                         \
        x(INT);                                                                \
        x(DOUBLE);                                                             \
        x(BOOL);                                                               \
        x(STRING);                                                             \
        x(CHAR);                                                               \
        return Opcode::NONE;                                                   \
    }

#define x(type_)              \
    if (type == type_##_TYPE) \
        return isGlobal ? Opcode::GET_##type_##_GLOBAL : Opcode::GET_##type_;

GET(x)
#undef x

#define ASSIGN(x)                                                                 \
    inline Opcode GetAssignInstruction(const TypeData &type, const bool isGlobal) \
    {                                                                             \
        x(INT);                                                                   \
        x(DOUBLE);                                                                \
        x(BOOL);                                                                  \
        x(STRING);                                                                \
        x(CHAR);                                                                  \
        if (type.isArray && isGlobal)                                             \
            return Opcode::ARRAY_ASSIGN_GLOBAL;                                   \
        if (type.isArray && !isGlobal)                                            \
            return Opcode::ARRAY_ASSIGN;                                          \
        if (type.type > NUM_DEF_TYPES - 1 && isGlobal)                            \
            return Opcode::STRUCT_ASSIGN_GLOBAL;                                  \
        if (type.type > NUM_DEF_TYPES - 1 && !isGlobal)                           \
            return Opcode::STRUCT_ASSIGN;                                         \
        else                                                                      \
            return Opcode::NONE;                                                  \
    }

#define x(type_)              \
    if (type == type_##_TYPE) \
        return isGlobal ? Opcode::type_##_##ASSIGN_GLOBAL : Opcode::type_##_##ASSIGN;

ASSIGN(x)
#undef x

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
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
        c.Symbols.UpdateSP(INT_SIZE);
    }
    else if (type == DOUBLE_TYPE)
    {
        cur->doubles.push_back(std::stod(literal));
        if (cur->doubles.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of double constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_DOUBLE, static_cast<oprand_t>(cur->doubles.size() - 1)});
        c.Symbols.UpdateSP(DOUBLE_SIZE);
    }
    else if (type == BOOL_TYPE)
    {
        cur->bools.push_back(literal == "true" ? true : false);
        if (cur->bools.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of bool constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_BOOL, static_cast<oprand_t>(cur->bools.size() - 1)});
        c.Symbols.UpdateSP(BOOL_SIZE);
    }
    else if (type == STRING_TYPE)
    {
        cur->strings.push_back(literal);
        if (cur->strings.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of string constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_STRING, static_cast<oprand_t>(cur->strings.size() - 1)});
        c.Symbols.UpdateSP(STRING_SIZE);
    }
    else if (type == CHAR_TYPE)
    {
        cur->chars.push_back(literal[0]);
        if (cur->chars.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of char constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_CHAR, static_cast<oprand_t>(cur->chars.size() - 1)});
        c.Symbols.UpdateSP(CHAR_SIZE);
    }
    else
        c.TypeError(l->Loc(), "Cannot have a literal of type " + ToString(l->t));
}

void NodeCompiler::CompileUnary(Unary *u, Compiler &c)
{
    u->right->NodeCompile(c);
    c.AddCode({TokenToOpcode(VOID_TYPE, u->op.type, u->right->GetType(), true), 1});
}

void NodeCompiler::CompileBinary(Binary *b, Compiler &c)
{
    b->left->NodeCompile(c);
    b->right->NodeCompile(c);
    c.AddCode({TokenToOpcode(b->left->GetType(), b->op.type, b->right->GetType(), false), 0});
}

void NodeCompiler::CompileAssign(Assign *a, Compiler &c)
{
    /*
    Valid assignment targets
    1) VarReference
    2) ArrayIndex
    3) FieldAccess
    */

    a->val->NodeCompile(c);

    VarReference *targetAsVR = dynamic_cast<VarReference *>(a->target.get());
    if (targetAsVR != nullptr)
    {
        VarID *vid = c.Symbols.GetVar(targetAsVR->name);

        size_t varStackLoc = c.Symbols.GetVariableStackLoc(targetAsVR->name);
        if (varStackLoc > MAX_OPRAND)
            c.CompileError(targetAsVR->Loc(), "Too many variables");

        if (vid->depth == 0)
            c.AddCode({GetAssignInstruction(vid->type, true), static_cast<oprand_t>(varStackLoc)});
        else
            c.AddCode({GetAssignInstruction(vid->type, false), static_cast<oprand_t>(varStackLoc)});
    }

    ArrayIndex *targetAsAI = dynamic_cast<ArrayIndex *>(a->target.get());
    if (targetAsAI != nullptr)
    {
        targetAsAI->name->NodeCompile(c);
        TypeData name = targetAsAI->name->GetType();

        targetAsAI->index->NodeCompile(c);

        if (name.isArray)
        {
            c.AddCode({Opcode::ARR_SET, 0});
            --name.type;
            c.AddCode({Opcode::PUSH, c.Symbols.SizeOf(name)});
        }
        else
            c.AddCode({Opcode::STRING_SET, 0});
    }

    FieldAccess *targetAsFA = dynamic_cast<FieldAccess *>(a->target.get());
    if (targetAsFA != nullptr)
    {
        targetAsFA->accessor->NodeCompile(c);
        StructID *sid = c.Symbols.GetStruct(targetAsFA->accessee->GetType());

        VarReference *vrAccessee = dynamic_cast<VarReference *>(targetAsFA->accessee.get());
        size_t offset = SIZE_MAX;
        for (const auto &member : sid->nameTypes)
        {
            offset += c.Symbols.SizeOf(member.second);
            if (member.first == vrAccessee->name)
                break;
        }

        c.AddCode({Opcode::STRUCT_MEMBER_SET, static_cast<oprand_t>(offset)});
    }
}

void NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    VarID *vid = c.Symbols.GetVar(vr->name);
    size_t stackLoc = c.Symbols.GetVariableStackLoc(vr->name);
    if (stackLoc > MAX_OPRAND)
        c.CompileError(vr->Loc(), "Too many variables, maximum number is " + std::to_string(MAX_OPRAND));

    if (vid->depth > 0)
        c.AddCode({GetGETInstruction(vid->type, false), static_cast<oprand_t>(stackLoc)});
    else
        c.AddCode({GetGETInstruction(vid->type, true), static_cast<oprand_t>(stackLoc)});
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{
    std::vector<TypeData> args;
    for (auto &e : fc->args)
        args.push_back(e->GetType());

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

    ERROR_GUARD(
        {
            for (auto &e : fc->args)
                e->NodeCompile(c);
        },
        c)

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
        c.AddCode({Opcode::CALL_LIBRARY_FUNC, static_cast<oprand_t>(funcNum)});
        break;
    }
    case FunctionType::NATIVE:
    {
        size_t funcNum = c.Symbols.GetNativeFuncNum(fid);
        if (funcNum > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many C library functions, maximum number is " + std::to_string(MAX_OPRAND));

        size_t argSize = 0;
        for (const auto &type : args)
            argSize += c.Symbols.SizeOf(type);

        c.AddCode({Opcode::PUSH, static_cast<oprand_t>(argSize)});
        c.AddCode({Opcode::NATIVE_CALL, static_cast<oprand_t>(funcNum)});
        break;
    }
    }
}

void NodeCompiler::CompileArrayIndex(ArrayIndex *ai, Compiler &c)
{
    ai->name->NodeCompile(c);
    TypeData name = ai->name->GetType();
    ai->index->NodeCompile(c);

    if (name.isArray)
    {
        --name.isArray;
        size_t elementSize = c.Symbols.SizeOf(name);
        c.AddCode({Opcode::PUSH, elementSize});
        c.AddCode({Opcode::ARR_INDEX, 0});
    }
    else
        c.AddCode({Opcode::STRING_INDEX, 0});
}

void NodeCompiler::CompileBracedInitialiser(BracedInitialiser *bi, Compiler &c)
{
    c.AddCode({Opcode::PUSH_SP_OFFSET, 8});
    ERROR_GUARD(
        {
            for (auto &e : bi->init)
                e->NodeCompile(c);
        },
        c)
    if (bi->GetType().isArray)
        c.Symbols.UpdateSP(ARRAY_SIZE);
    else
        c.Symbols.UpdateSP(STRUCT_SIZE);
}

void NodeCompiler::CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c)
{
    da->size->NodeCompile(c);
    TypeData elementType = da->GetType();
    elementType.isArray--;
    size_t elementSize = c.Symbols.SizeOf(elementType);
    c.AddCode({Opcode::PUSH, static_cast<oprand_t>(elementSize)});
    c.AddCode({Opcode::ARR_ALLOC, 0});
    c.Symbols.UpdateSP(ARRAY_SIZE);
}

void NodeCompiler::CompileFieldAccess(FieldAccess *fa, Compiler &c)
{
    fa->accessor->NodeCompile(c);
    TypeData accessor = fa->accessor->GetType();

    StructID *sid = c.Symbols.GetStruct(accessor);
    if (sid == nullptr)
        c.TypeError(fa->Loc(), "Type " + ToString(accessor) + " cannot be accessed into");

    VarReference *vAccessee = dynamic_cast<VarReference *>(fa->accessee.get());

    // index of accessee in the underlying array
    size_t offset = 0;
    for (const auto &member : sid->nameTypes)
    {
        if (member.first == vAccessee->name)
            break;
        offset += c.Symbols.SizeOf(member.second);
    }

    c.AddCode({Opcode::STRUCT_MEMBER, static_cast<oprand_t>(offset)});
}

void NodeCompiler::CompileTypeCast(TypeCast *tc, Compiler &c)
{
    tc->arg->NodeCompile(c);
    TypeData old = tc->arg->GetType();
    TypeData nw = tc->t;

    bool isDownCast = c.Symbols.CanAssign(nw, old);
    bool isUpCast = c.Symbols.CanAssign(old, nw);

    if (!isDownCast && !isUpCast)
        c.TypeError(tc->Loc(), "Cannot cast " + ToString(old) + " to " + ToString(nw));

    c.AddCode({Opcode::CAST, tc->t.type});
}

void NodeCompiler::CompileSequence(Sequence *, Compiler &)
{
    return;
}

//------------------STATEMENTS---------------------//

void NodeCompiler::CompileExprStmt(ExprStmt *es, Compiler &c)
{
    es->exp->NodeCompile(c);
    TypeData exp = es->exp->GetType();
    if (exp != VOID_TYPE)
    {
        c.AddCode({Opcode::POP, c.Symbols.SizeOf(exp)});
    }
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    size_t size = 0;
    if (dv->value != nullptr)
    {
        size_t beginning = c.Symbols.GetCurOffset();
        dv->value->NodeCompile(c);
        size = c.Symbols.GetCurOffset() - beginning;

        if (c.Symbols.depth == 0)
            c.AddCode({GetAssignInstruction(dv->t, true), 0});
    }
    else
    {
        if (c.Symbols.depth == 0)
            c.SymbolError(dv->Loc(), "Global variable must be initialised");
        size = c.Symbols.SizeOf(dv->t);
    }
    c.Symbols.AddVar(dv->t, dv->name, size);
}

void NodeCompiler::CompileBlock(Block *b, Compiler &c)
{
    c.Symbols.depth++;

    ERROR_GUARD(
        {
            for (auto &stmt : b->stmts)
                stmt->NodeCompile(c);
        },
        c)

    c.ClearCurrentDepthWithPOPInst();
    c.Symbols.depth--;
}

void NodeCompiler::CompileIfStmt(IfStmt *i, Compiler &c)
{
    i->cond->NodeCompile(c);

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

    ws->cond->NodeCompile(c);

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
    c.AddFunction();

    c.cur->arity = fd->params.size();

    std::vector<TypeData> argtypes;
    for (auto &arg : fd->params)
        argtypes.push_back(arg.first);

    std::vector<TypeData> templates;
    for (auto &t : fd->templates)
        templates.push_back(t.first);

    FuncID *isThere = c.Symbols.GetFunc(fd->name, templates, argtypes);
    if (isThere != nullptr)
    {
        std::ostringstream out;

        if (fd->templates.size() > 0)
        {
            out << "template<|";
            for (size_t i = 0; i < fd->templates.size(); i++)
            {
                out << fd->templates[i].second;
                if (i != fd->templates.size() - 1)
                    out << ", ";
            }
            out << "|>";
        }

        out << fd->ret << " ";
        out << fd->name;

        out << "(";
        if (fd->params.size() > 0)
        {
            for (size_t i = 0; i < fd->params.size(); i++)
            {
                out << fd->params[i].first << " " << fd->params[i].second;
                if (i != fd->params.size() - 1)
                    out << ", ";
            }
        }
        out << ")";
        c.SymbolError(fd->Loc(), "Function '" + out.str() + "' has already been defined");
    }

    c.Symbols.AddFunc(FuncID(fd->ret, fd->name, templates, argtypes, FunctionType::USER_DEFINED, c.parseIndex));

    if (fd->templates.size() > 0)
        return;

    c.Symbols.depth++;
    for (auto &arg : fd->params)
        c.Symbols.AddVar(arg.first, arg.second, c.Symbols.SizeOf(arg.first));

    ERROR_GUARD(
        {
            for (auto &stmt : fd->body)
                stmt->NodeCompile(c);
        },
        c)

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
    std::vector<SP<Expr>> init;
    std::vector<std::pair<std::string, TypeData>> nameTypes;

    if (parent != VOID_TYPE)
    {
        if (parent.isArray)
            c.TypeError(sd->Loc(), "Parent of a struct cannot be array");
        StructID *sidParent = c.Symbols.GetStruct(parent);

        if (sidParent == nullptr)
            c.TypeError(sd->Loc(), "Invalid parent struct name");

        for (const auto &kv : sidParent->nameTypes)
            nameTypes.push_back({kv.first, kv.second});
    }

    ERROR_GUARD(
        {
            for (auto &d : sd->decls)
            {
                DeclaredVar *asDV = dynamic_cast<DeclaredVar *>(d.get());
                if (asDV == nullptr)
                    c.TypeError(d->Loc(), "The body of struct declarations can only consist of variable declarations");

                if (asDV->value != nullptr)
                    c.CompileError(asDV->value->Loc(), "Variable declarations inside struct declarations cannot have values");

                memberNames.push_back(asDV->name);
                memTypes.push_back(asDV->t);
                nameTypes.push_back({asDV->name, asDV->t});
            }
        },
        c)

    c.Symbols.AddStruct(StructID(name, type, parent, nameTypes));
}

void NodeCompiler::CompileImportStmt(ImportStmt *is, Compiler &c)
{
    std::vector<std::string> libraryFuncs;
    assert(is->libraries.size() > 0);
    ERROR_GUARD(
        {
            for (const auto library : is->libraries)
            {
                libraryFuncs = c.Symbols.GetLibraryFunctionNames(library);
                for (auto &lf : libraryFuncs)
                {
                    FuncID func = c.Symbols.ParseLibraryFunction(lf, FunctionType::LIBRARY);
                    c.Symbols.AddCLibFunc(func);

                    if (c.Symbols.NumCFuncs() > MAX_OPRAND)
                        c.CompileError(is->Loc(), "Cannot import more than " + std::to_string(MAX_OPRAND) + " library functions in total");

                    c.libfuncs.push_back(LibraryFunctionDef(func.name, library, func.argtypes.size()));
                }
            }
        },
        c)
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

void NodeCompiler::CompileTryCatch(TryCatch *tc, Compiler &c)
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
    c.Symbols.AddVar(catchType, catchVarName, c.Symbols.SizeOf(catchType));
    tc->catchClause->NodeCompile(c);
}

//-----------------EXPRESSIONS---------------------//

void Literal::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileLiteral(this, c);
}

void Unary::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileUnary(this, c);
}

void Binary::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileBinary(this, c);
}

void Assign::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileAssign(this, c);
}

void VarReference::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileVarReference(this, c);
}

void FunctionCall::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileFunctionCall(this, c);
}

void ArrayIndex::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileArrayIndex(this, c);
}

void BracedInitialiser::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileBracedInitialiser(this, c);
}

void DynamicAllocArray::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileDynamicAllocArray(this, c);
}

void FieldAccess::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileFieldAccess(this, c);
}

void TypeCast::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileTypeCast(this, c);
}

void Sequence::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileSequence(this, c);
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
    NodeCompiler::CompileTryCatch(this, c);
}