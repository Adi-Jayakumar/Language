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

Opcode TokenToOpcode(TypeData l, TokenID t, TypeData r, bool is_unary)
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
        if (is_unary)
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

#define GET(x)                                                                  \
    inline Opcode GetGETInstruction(const TypeData &type, const bool is_global) \
    {                                                                           \
                                                                                \
        if (type.is_array && is_global)                                         \
            return Opcode::GET_ARRAY_GLOBAL;                                    \
        else if (type.is_array && !is_global)                                   \
            return Opcode::GET_ARRAY;                                           \
        else if (type.type > (NUM_DEF_TYPES - 1) && is_global)                  \
            return Opcode::GET_STRUCT_GLOBAL;                                   \
        else if (type.type > (NUM_DEF_TYPES - 1) && !is_global)                 \
            return Opcode::GET_STRUCT;                                          \
        x(INT);                                                                 \
        x(DOUBLE);                                                              \
        x(BOOL);                                                                \
        x(STRING);                                                              \
        x(CHAR);                                                                \
        return Opcode::NONE;                                                    \
    }

#define x(type_)              \
    if (type == type_##_TYPE) \
        return is_global ? Opcode::GET_##type_##_GLOBAL : Opcode::GET_##type_;

GET(x)
#undef x

#define ASSIGN(x)                                                                  \
    inline Opcode GetAssignInstruction(const TypeData &type, const bool is_global) \
    {                                                                              \
        x(INT);                                                                    \
        x(DOUBLE);                                                                 \
        x(BOOL);                                                                   \
        x(STRING);                                                                 \
        x(CHAR);                                                                   \
        if (type.is_array && is_global)                                            \
            return Opcode::ARRAY_ASSIGN_GLOBAL;                                    \
        if (type.is_array && !is_global)                                           \
            return Opcode::ARRAY_ASSIGN;                                           \
        if (type.type > NUM_DEF_TYPES - 1 && is_global)                            \
            return Opcode::STRUCT_ASSIGN_GLOBAL;                                   \
        if (type.type > NUM_DEF_TYPES - 1 && !is_global)                           \
            return Opcode::STRUCT_ASSIGN;                                          \
        else                                                                       \
            return Opcode::NONE;                                                   \
    }

#define x(type_)              \
    if (type == type_##_TYPE) \
        return is_global ? Opcode::type_##_##ASSIGN_GLOBAL : Opcode::type_##_##ASSIGN;

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
        c.symbols.UpdateSP(INT_SIZE);
    }
    else if (type == DOUBLE_TYPE)
    {
        cur->doubles.push_back(std::stod(literal));
        if (cur->doubles.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of double constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_DOUBLE, static_cast<oprand_t>(cur->doubles.size() - 1)});
        c.symbols.UpdateSP(DOUBLE_SIZE);
    }
    else if (type == BOOL_TYPE)
    {
        cur->bools.push_back(literal == "true" ? true : false);
        if (cur->bools.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of bool constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_BOOL, static_cast<oprand_t>(cur->bools.size() - 1)});
        c.symbols.UpdateSP(BOOL_SIZE);
    }
    else if (type == STRING_TYPE)
    {
        cur->strings.push_back(literal);
        if (cur->strings.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of string constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_STRING, static_cast<oprand_t>(cur->strings.size() - 1)});
        c.symbols.UpdateSP(STRING_SIZE);
    }
    else if (type == CHAR_TYPE)
    {
        cur->chars.push_back(literal[0]);
        if (cur->chars.size() > MAX_OPRAND)
            c.CompileError(l->Loc(), "Max number of char constants is " + std::to_string(MAX_OPRAND));

        c.AddCode({Opcode::LOAD_CHAR, static_cast<oprand_t>(cur->chars.size() - 1)});
        c.symbols.UpdateSP(CHAR_SIZE);
    }
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

    VarReference *target_as_vr = dynamic_cast<VarReference *>(a->target.get());
    if (target_as_vr != nullptr)
    {
        std::optional<VarID> vid = c.symbols.GetVar(target_as_vr->name);

        size_t var_stack_loc = c.symbols.GetVariableStackLoc(target_as_vr->name);
        if (var_stack_loc > MAX_OPRAND)
            c.CompileError(target_as_vr->Loc(), "Too many variables");

        if (vid->depth == 0)
            c.AddCode({GetAssignInstruction(vid->type, true), static_cast<oprand_t>(var_stack_loc)});
        else
            c.AddCode({GetAssignInstruction(vid->type, false), static_cast<oprand_t>(var_stack_loc)});
    }

    ArrayIndex *target_as_ai = dynamic_cast<ArrayIndex *>(a->target.get());
    if (target_as_ai != nullptr)
    {
        target_as_ai->name->NodeCompile(c);
        TypeData name = target_as_ai->name->GetType();

        target_as_ai->index->NodeCompile(c);

        if (name.is_array)
        {
            c.AddCode({Opcode::ARR_SET, 0});
            --name.type;
            c.AddCode({Opcode::PUSH, c.symbols.SizeOf(name)});
        }
        else
            c.AddCode({Opcode::STRING_SET, 0});
    }

    FieldAccess *target_as_fa = dynamic_cast<FieldAccess *>(a->target.get());
    if (target_as_fa != nullptr)
    {
        target_as_fa->accessor->NodeCompile(c);
        std::optional<StructID> sid = c.symbols.GetStruct(target_as_fa->accessee->GetType());

        VarReference *vr_accessee = dynamic_cast<VarReference *>(target_as_fa->accessee.get());
        size_t offset = SIZE_MAX;
        for (const auto &member : sid->nameTypes)
        {
            offset += c.symbols.SizeOf(member.second);
            if (member.first == vr_accessee->name)
                break;
        }

        c.AddCode({Opcode::STRUCT_MEMBER_SET, static_cast<oprand_t>(offset)});
    }
}

void NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    std::optional<VarID> vid = c.symbols.GetVar(vr->name);
    size_t stack_loc = c.symbols.GetVariableStackLoc(vr->name);
    if (stack_loc > MAX_OPRAND)
        c.CompileError(vr->Loc(), "Too many variables, maximum number is " + std::to_string(MAX_OPRAND));

    if (vid->depth > 0)
        c.AddCode({GetGETInstruction(vid->type, false), static_cast<oprand_t>(stack_loc)});
    else
        c.AddCode({GetGETInstruction(vid->type, true), static_cast<oprand_t>(stack_loc)});
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{
    std::vector<TypeData> args;
    for (auto &e : fc->args)
        args.push_back(e->GetType());

    std::optional<FuncID> fid = c.symbols.GetFunc(fc->name, args);

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
        size_t func_num = c.symbols.GetUDFuncNum(fid);
        if (func_num > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many functions, maximum number is " + std::to_string(MAX_OPRAND));
        c.AddCode({Opcode::CALL_F, static_cast<oprand_t>(func_num + 1)});
        break;
    }
    case FunctionType::USER_DEFINED_TEMPLATE:
    {
        break;
    }
    case FunctionType::LIBRARY:
    {
        size_t func_num = c.symbols.GetCLibFuncNum(fid);
        if (func_num > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many C library functions, maximum number is " + std::to_string(MAX_OPRAND));
        c.AddCode({Opcode::CALL_LIBRARY_FUNC, static_cast<oprand_t>(func_num)});
        break;
    }
    case FunctionType::NATIVE:
    {
        size_t func_num = c.symbols.GetNativeFuncNum(fid);
        if (func_num > MAX_OPRAND - 1)
            c.CompileError(fc->Loc(), "Too many C library functions, maximum number is " + std::to_string(MAX_OPRAND));

        size_t arg_size = 0;
        for (const auto &type : args)
            arg_size += c.symbols.SizeOf(type);

        c.AddCode({Opcode::PUSH, static_cast<oprand_t>(arg_size)});
        c.AddCode({Opcode::NATIVE_CALL, static_cast<oprand_t>(func_num)});
        break;
    }
    }
}

void NodeCompiler::CompileArrayIndex(ArrayIndex *ai, Compiler &c)
{
    ai->name->NodeCompile(c);
    TypeData name = ai->name->GetType();
    ai->index->NodeCompile(c);

    if (name.is_array)
    {
        --name.is_array;
        size_t element_size = c.symbols.SizeOf(name);
        c.AddCode({Opcode::PUSH, element_size});
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
    if (bi->GetType().is_array)
        c.symbols.UpdateSP(ARRAY_SIZE);
    else
        c.symbols.UpdateSP(STRUCT_SIZE);
}

void NodeCompiler::CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c)
{
    da->size->NodeCompile(c);
    TypeData element_type = da->GetType();
    element_type.is_array--;
    size_t elementSize = c.symbols.SizeOf(element_type);
    c.AddCode({Opcode::PUSH, static_cast<oprand_t>(elementSize)});
    c.AddCode({Opcode::ARR_ALLOC, 0});
    c.symbols.UpdateSP(ARRAY_SIZE);
}

void NodeCompiler::CompileFieldAccess(FieldAccess *fa, Compiler &c)
{
    fa->accessor->NodeCompile(c);
    TypeData accessor = fa->accessor->GetType();

    std::optional<StructID> sid = c.symbols.GetStruct(accessor);
    VarReference *vr_accessee = dynamic_cast<VarReference *>(fa->accessee.get());

    // index of accessee in the underlying array
    size_t offset = 0;
    for (const auto &member : sid->nameTypes)
    {
        if (member.first == vr_accessee->name)
            break;
        offset += c.symbols.SizeOf(member.second);
    }

    c.AddCode({Opcode::STRUCT_MEMBER, static_cast<oprand_t>(offset)});
}

void NodeCompiler::CompileTypeCast(TypeCast *tc, Compiler &c)
{
    tc->arg->NodeCompile(c);
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
        c.AddCode({Opcode::POP, c.symbols.SizeOf(exp)});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    size_t size = 0;
    if (dv->value != nullptr)
    {
        size_t beginning = c.symbols.GetCurOffset();
        dv->value->NodeCompile(c);
        size = c.symbols.GetCurOffset() - beginning;

        if (c.symbols.depth == 0)
            c.AddCode({GetAssignInstruction(dv->t, true), 0});
    }
    else
        size = c.symbols.SizeOf(dv->t);
    c.symbols.AddVar(dv->t, dv->name, size);
}

void NodeCompiler::CompileBlock(Block *b, Compiler &c)
{
    c.symbols.depth++;

    ERROR_GUARD(
        {
            for (auto &stmt : b->stmts)
                stmt->NodeCompile(c);
        },
        c)

    c.ClearCurrentDepthWithPOPInst();
    c.symbols.depth--;
}

void NodeCompiler::CompileIfStmt(IfStmt *i, Compiler &c)
{
    i->cond->NodeCompile(c);

    c.AddCode({Opcode::GOTO_LABEL_IF_FALSE, 0});
    std::pair<size_t, size_t> not_true = c.LastAddedCodeLoc();

    c.AddCode({Opcode::GOTO_LABEL, 0});
    std::pair<size_t, size_t> isTrue = c.LastAddedCodeLoc();

    c.AddRoutine();
    i->then_branch->NodeCompile(c);
    size_t then_routine = c.GetCurRoutineIndex();

    if (then_routine > MAX_OPRAND)
        c.CompileError(i->then_branch->Loc(), "Too many routines");
    // isTrue->op = static_cast<oprand_t>(thenRoutine);
    c.ModifyOprandAt(isTrue, static_cast<oprand_t>(then_routine));

    c.AddCode({Opcode::GOTO_LABEL, 0});
    std::pair<size_t, size_t> thenReturn = c.LastAddedCodeLoc();

    if (i->else_branch == nullptr)
    {
        c.AddRoutine();
        size_t new_routine = c.GetCurRoutineIndex();
        if (new_routine > MAX_OPRAND)
            c.CompileError(i->then_branch->Loc(), "Too many routines");
        c.ModifyOprandAt(thenReturn, static_cast<oprand_t>(new_routine));
        c.ModifyOprandAt(not_true, static_cast<oprand_t>(new_routine));
    }
    else
    {
        c.AddRoutine();
        i->else_branch->NodeCompile(c);

        size_t else_index = c.GetCurRoutineIndex();
        if (else_index > MAX_OPRAND)
            c.CompileError(i->else_branch->Loc(), "Too many routines");
        c.ModifyOprandAt(not_true, static_cast<oprand_t>(else_index));

        c.AddCode({Opcode::GOTO_LABEL, 0});
        std::pair<size_t, size_t> else_return = c.LastAddedCodeLoc();

        c.AddRoutine();
        size_t new_routine = c.GetCurRoutineIndex();
        if (new_routine > MAX_OPRAND)
            c.CompileError(i->else_branch->Loc(), "Too many routines");
        c.ModifyOprandAt(thenReturn, static_cast<oprand_t>(new_routine));
        c.ModifyOprandAt(else_return, static_cast<oprand_t>(new_routine));
    }
}

void NodeCompiler::CompileWhileStmt(WhileStmt *ws, Compiler &c)
{
    c.AddCode({Opcode::GOTO_LABEL, 0});
    std::pair<size_t, size_t> enter = c.LastAddedCodeLoc();

    c.AddRoutine();
    size_t loop_routine = c.GetCurRoutineIndex();
    if (loop_routine > MAX_OPRAND)
        c.CompileError(ws->Loc(), "Too many routines");

    c.ModifyOprandAt(enter, static_cast<oprand_t>(loop_routine));

    ws->cond->NodeCompile(c);

    c.AddCode({Opcode::GOTO_LABEL_IF_FALSE, 0});
    std::pair<size_t, size_t> not_true = c.LastAddedCodeLoc();

    ws->body->NodeCompile(c);
    c.AddCode({Opcode::GOTO_LABEL, static_cast<oprand_t>(loop_routine)});

    c.AddRoutine();
    size_t new_routine = c.GetCurRoutineIndex();
    if (new_routine > MAX_OPRAND)
        c.CompileError(ws->Loc(), "Too many routines");

    c.ModifyOprandAt(not_true, static_cast<oprand_t>(new_routine));
}

void NodeCompiler::CompileFuncDecl(FuncDecl *fd, Compiler &c)
{
    c.cur_func = fd;

    c.AddFunction();

    c.cur->arity = fd->params.size();

    std::vector<TypeData> argtypes;
    for (auto &arg : fd->params)
        argtypes.push_back(arg.first);

    c.symbols.AddFunc(FuncID(fd->ret, fd->name, argtypes, FunctionType::USER_DEFINED, c.parse_index));

    c.symbols.depth++;
    for (auto &arg : fd->params)
        c.symbols.AddVar(arg.first, arg.second, c.symbols.SizeOf(arg.first));

    ERROR_GUARD(
        {
            for (auto &stmt : fd->body)
                stmt->NodeCompile(c);
        },
        c)

    c.ClearCurrentDepthWithPOPInst();
    c.symbols.depth--;
    c.cur = &c.functions[0];

    c.cur_func = nullptr;
}

void NodeCompiler::CompileReturn(Return *r, Compiler &c)
{
    if (r->ret_val == nullptr)
        c.AddCode({Opcode::RETURN_VOID, 0});
    else
    {
        // TODO - check return type can be assigned to one
        // specified at function declaration
        r->ret_val->NodeCompile(c);
        c.AddCode({Opcode::RETURN, 0});
    }
}

void NodeCompiler::CompileStructDecl(StructDecl *sd, Compiler &c)
{
    TypeData type = c.symbols.ResolveType(sd->name).value();

    TypeData parent = sd->parent;
    std::vector<std::string> member_names;
    std::vector<TypeData> mem_types;
    std::vector<SP<Expr>> init;
    std::vector<std::pair<std::string, TypeData>> name_types;

    if (parent != VOID_TYPE)
    {
        std::optional<StructID> sid_parent = c.symbols.GetStruct(parent);
        for (const auto &kv : sid_parent->nameTypes)
            name_types.push_back({kv.first, kv.second});
    }

    for (auto &d : sd->decls)
    {
        DeclaredVar *as_dv = dynamic_cast<DeclaredVar *>(d.get());
        member_names.push_back(as_dv->name);
        mem_types.push_back(as_dv->t);
        name_types.push_back({as_dv->name, as_dv->t});
    }

    c.symbols.AddStruct(StructID(sd->name, type, parent, name_types));
}

void NodeCompiler::CompileImportStmt(ImportStmt *is, Compiler &c)
{
    std::vector<std::string> library_funcs;
    assert(is->libraries.size() > 0);
    ERROR_GUARD(
        {
            for (const auto library : is->libraries)
            {
                library_funcs = c.symbols.GetLibraryFunctionNames(library);
                for (auto &lf : library_funcs)
                {
                    FuncID func = c.symbols.ParseLibraryFunction(lf, FunctionType::LIBRARY);
                    c.symbols.AddCLibFunc(func);

                    if (c.symbols.NumCFuncs() > MAX_OPRAND)
                        c.CompileError(is->Loc(), "Cannot import more than " + std::to_string(MAX_OPRAND) + " library functions in total");

                    c.lib_funcs.push_back(LibraryFunctionDef(func.name, library, func.argtypes.size()));
                }
            }
        },
        c)
}

void NodeCompiler::CompileBreak(Break *b, Compiler &c)
{
    if (c.break_indices.size() == 0)
        c.CompileError(b->Loc(), "Break statement cannot occur outside of a loop");

    std::vector<size_t> *cur_loop_breaks = &c.break_indices.top();
    size_t breakLoc = c.CodeSize();

    cur_loop_breaks->push_back(breakLoc);
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
    ti.func = c.cur - &c.functions[0];

    TypeData catch_type = tc->catch_var.first;
    std::string catch_var_name = tc->catch_var.second;

    ti.type = catch_type.type;
    ti.is_array = catch_type.is_array;

    size_t throw_info_size = c.throw_stack.size();
    if (throw_info_size > MAX_OPRAND)
        c.CompileError(tc->Loc(), "Too many try-catch blocks, maximum number is " + std::to_string(MAX_OPRAND));

    c.AddCode({Opcode::PUSH_THROW_INFO, static_cast<oprand_t>(throw_info_size)});
    tc->try_clause->NodeCompile(c);

    size_t s_index = c.CodeSize();
    if (s_index > MAX_OPRAND)
        c.CompileError(tc->try_clause->Loc(), "Too much code generated from 'try' clause");

    ti.index = static_cast<oprand_t>(s_index);
    c.throw_stack.push_back(ti);
    c.symbols.AddVar(catch_type, catch_var_name, c.symbols.SizeOf(catch_type));
    tc->catch_clause->NodeCompile(c);
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