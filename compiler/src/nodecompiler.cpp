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
    else if (t == TokenID::BANG)
    {
        return Opcode::BANG;
    }
    else
        return Opcode::NONE;
}

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    TypeID type = l->GetType().type;
    std::string literal = l->Loc().literal;
    Function *cur = c.cur;

    if (type == 1)
    {
        cur->ints.push_back(std::stoi(literal));
        if (cur->ints.size() > UINT8_MAX)
            c.CompileError(l->Loc(), "Max number of int constants is " + std::to_string(UINT8_MAX));

        cur->code.push_back({Opcode::LOAD_INT, static_cast<uint8_t>(cur->ints.size() - 1)});
    }
    else if (type == 2)
    {
        cur->doubles.push_back(std::stod(literal));
        if (cur->doubles.size() > UINT8_MAX)
            c.CompileError(l->Loc(), "Max number of double constants is " + std::to_string(UINT8_MAX));

        cur->code.push_back({Opcode::LOAD_DOUBLE, static_cast<uint8_t>(cur->doubles.size() - 1)});
    }
    else if (type == 3)
    {
        cur->bools.push_back(literal == "true" ? true : false);
        if (cur->bools.size() > UINT8_MAX)
            c.CompileError(l->Loc(), "Max number of bool constants is " + std::to_string(UINT8_MAX));

        cur->code.push_back({Opcode::LOAD_BOOL, static_cast<uint8_t>(cur->bools.size() - 1)});
    }
    else if (type == 4)
    {
        cur->strings.push_back(literal);
        if (cur->strings.size() > UINT8_MAX)
            c.CompileError(l->Loc(), "Max number of string constants is " + std::to_string(UINT8_MAX));

        cur->code.push_back({Opcode::LOAD_STRING, static_cast<uint8_t>(cur->strings.size() - 1)});
    }
    else if (type == 5)
    {
        cur->chars.push_back(literal[0]);
        if (cur->chars.size() > UINT8_MAX)
            c.CompileError(l->Loc(), "Max number of char constants is " + std::to_string(UINT8_MAX));

        cur->code.push_back({Opcode::LOAD_CHAR, static_cast<uint8_t>(cur->chars.size() - 1)});
    }
}

void NodeCompiler::CompileUnary(Unary *u, Compiler &c)
{
    u->right->NodeCompile(c);
    c.cur->code.push_back({TokenToOpcode({0, false}, u->op.type, u->right->GetType(), true), 1});
}

void NodeCompiler::CompileBinary(Binary *b, Compiler &c)
{
    b->left->NodeCompile(c);
    b->right->NodeCompile(c);
    c.cur->code.push_back({TokenToOpcode(b->left->GetType(), b->op.type, b->right->GetType(), false), 0});
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
        size_t varStackLoc = c.Symbols.FindVarByName(targetAsVR->name);
        if (c.Symbols.vars[varStackLoc].depth == 0)
            c.cur->code.push_back({Opcode::VAR_A_GLOBAL, static_cast<uint8_t>(varStackLoc)});
        else
            c.cur->code.push_back({Opcode::VAR_A, static_cast<uint8_t>(varStackLoc)});
    }

    ArrayIndex *targetAsAI = dynamic_cast<ArrayIndex *>(a->target.get());
    if (targetAsAI != nullptr)
    {
        targetAsAI->name->NodeCompile(c);
        targetAsAI->index->NodeCompile(c);
        c.cur->code.push_back({Opcode::ARR_SET, 0});
    }

    FieldAccess *targetAsFA = dynamic_cast<FieldAccess *>(a->target.get());
    if (targetAsFA != nullptr)
    {
        TypeData strct = targetAsFA->accessor->t;
        size_t index = c.Symbols.FindStruct(strct);
        StructID sID = c.Symbols.strcts[index];

        VarReference *acessee = dynamic_cast<VarReference *>(targetAsFA->accessee.get());
        if (acessee == nullptr)
            c.CompileError(targetAsFA->accessee->Loc(), "Invalid struct accessor");

        targetAsFA->accessor->NodeCompile(c);

        size_t strctMem = SIZE_MAX;
        for (size_t i = 0; i < sID.memberNames.size(); i++)
        {
            if (sID.memberNames[i] == acessee->name)
            {
                strctMem = i;
                break;
            }
        }

        c.cur->code.push_back({Opcode::STRUCT_MEMBER_SET, static_cast<uint8_t>(strctMem)});
    }
}

void NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    uint8_t varStackLoc = static_cast<uint8_t>(c.Symbols.FindVarByName(vr->name));
    if (c.Symbols.vars[varStackLoc].depth == 0)
        c.cur->code.push_back({Opcode::GET_V_GLOBAL, varStackLoc});
    else
        c.cur->code.push_back({Opcode::GET_V, varStackLoc});
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{
    std::vector<TypeData> argtypes;

    for (auto &arg : fc->args)
    {
        argtypes.push_back(arg->GetType());
        arg->NodeCompile(c);
    }

    size_t index = c.Symbols.FindFunc(fc->name, argtypes);
    if (index != SIZE_MAX)
    {
        if (!c.Symbols.funcs[index].isLibFunc)
            c.cur->code.push_back({Opcode::CALL_F, static_cast<uint8_t>(1 + index)});
        else
        {
            uint8_t libindex = static_cast<uint8_t>(c.Symbols.funcs[index].isLibFunc - 1);
            c.cur->code.push_back({Opcode::CALL_LIBRARY_FUNC, static_cast<uint8_t>(libindex)});
        }
    }
    else
    {
        size_t natFunc = c.Symbols.FindNativeFunctions(argtypes, fc->name);
        if (natFunc == 0)
        {
            size_t arity = argtypes.size();
            if (arity > UINT8_MAX)
                c.CompileError(fc->Loc(), "Maximum number arguments to Print call is " + std::to_string(UINT8_MAX));
            c.cur->code.push_back({Opcode::PRINT, static_cast<uint8_t>(arity)});
        }
        else
            c.cur->code.push_back({Opcode::NATIVE_CALL, static_cast<uint8_t>(natFunc)});
    }
}

void NodeCompiler::CompileArrayIndex(ArrayIndex *ai, Compiler &c)
{
    ai->name->NodeCompile(c);
    ai->index->NodeCompile(c);
    c.cur->code.push_back({Opcode::ARR_INDEX, 0});
}

void NodeCompiler::CompileBracedInitialiser(BracedInitialiser *bi, Compiler &c)
{
    if (bi->size > UINT8_MAX)
        c.CompileError(bi->Loc(), "Inline arrays' max size is " + std::to_string(UINT8_MAX));

    if (bi->size > UINT8_MAX)
        c.CompileError(bi->Loc(), "Braced initialisers can only have " + std::to_string(UINT8_MAX) + " elements");

    if (bi->GetType().isArray)
        c.cur->code.push_back({Opcode::ARR_ALLOC, static_cast<uint8_t>(bi->size)});
    else
        c.cur->code.push_back({Opcode::STRUCT_ALLOC, static_cast<uint8_t>(bi->size)});

    for (auto &e : bi->init)
    {
        try
        {
            e->NodeCompile(c);
        }
        catch (const std::exception &e)
        {
            c.hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }
    if (bi->GetType().isArray)
        c.cur->code.push_back({Opcode::ARR_D, static_cast<uint8_t>(bi->size)});
    else
        c.cur->code.push_back({Opcode::STRUCT_D, static_cast<uint8_t>(bi->size)});
}

void NodeCompiler::CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c)
{
    da->size->NodeCompile(c);
    c.cur->code.push_back({Opcode::ARR_ALLOC, 0});
}

void NodeCompiler::CompileFieldAccess(FieldAccess *fa, Compiler &c)
{
    TypeData strct = fa->accessor->t;
    size_t index = c.Symbols.FindStruct(strct);
    std::cout << "Index " << index << std::endl;
    StructID sID = c.Symbols.strcts[index];

    VarReference *acessee = dynamic_cast<VarReference *>(fa->accessee.get());
    if (acessee == nullptr)
        c.CompileError(fa->accessee->Loc(), "Invalid struct accessor");

    fa->accessor->NodeCompile(c);

    size_t strctMem = SIZE_MAX;
    for (size_t i = 0; i < sID.memberNames.size(); i++)
    {
        if (sID.memberNames[i] == acessee->name)
        {
            strctMem = i;
            break;
        }
    }

    c.cur->code.push_back({Opcode::STRUCT_MEMBER, static_cast<uint8_t>(strctMem)});
}

void NodeCompiler::CompileTypeCast(TypeCast *tc, Compiler &c)
{
    tc->arg->NodeCompile(c);
    c.cur->code.push_back({Opcode::CAST, tc->type.type});
}

//------------------STATEMENTS---------------------//

void NodeCompiler::CompileExprStmt(ExprStmt *es, Compiler &c)
{
    es->exp->NodeCompile(c);
    FunctionCall *asFC = dynamic_cast<FunctionCall *>(es->exp.get());

    TypeData voidType = GetTypeNameMap()["void"];
    if (asFC == nullptr || asFC->GetType() != voidType)
        c.cur->code.push_back({Opcode::POP, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    if (c.Symbols.vars.size() > UINT8_MAX)
        c.CompileError(dv->Loc(), "Max number of variables in a Function is " + std::to_string(UINT8_MAX));

    c.Symbols.AddVar(dv->t, dv->name);
    dv->value->NodeCompile(c);

    if (c.Symbols.depth == 0)
        c.cur->code.push_back({Opcode::VAR_D_GLOBAL, 0});
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
    i->cond->NodeCompile(c);
    c.cur->code.push_back({Opcode::JUMP_IF_FALSE, 0});

    size_t patchIndex = c.cur->code.size() - 1;
    size_t befSize = c.cur->code.size();

    bool isNotBlock = dynamic_cast<Block *>(i->thenBranch.get()) == nullptr;

    if (isNotBlock)
        c.Symbols.depth++;

    i->thenBranch->NodeCompile(c);

    if (isNotBlock)
    {
        c.ClearCurrentDepthWithPOPInst();
        c.Symbols.depth--;
    }

    size_t sizeDiff = c.cur->code.size() - befSize;

    if (sizeDiff > UINT8_MAX)
        c.CompileError(i->Loc(), "Too much code to junmp over");

    c.cur->code[patchIndex].op = static_cast<uint8_t>(sizeDiff);

    if (i->elseBranch == nullptr)
        return;

    c.cur->code[patchIndex].op++;

    c.cur->code.push_back({Opcode::JUMP, 0});

    patchIndex = c.cur->code.size() - 1;
    befSize = c.cur->code.size();

    isNotBlock = dynamic_cast<Block *>(i->elseBranch.get()) == nullptr;

    if (isNotBlock)
        c.Symbols.depth++;

    i->elseBranch->NodeCompile(c);

    if (isNotBlock)
    {
        c.ClearCurrentDepthWithPOPInst();
        c.Symbols.depth--;
    }

    sizeDiff = c.cur->code.size() - befSize;
    if (sizeDiff > UINT8_MAX)
        c.CompileError(i->Loc(), "Too much code to junmp over");
    c.cur->code[patchIndex].op = static_cast<uint8_t>(sizeDiff);
}

void NodeCompiler::CompileWhileStmt(WhileStmt *ws, Compiler &c)
{
    size_t begLoop = c.cur->code.size() - 1;
    ws->cond->NodeCompile(c);
    c.cur->code.push_back({Opcode::JUMP_IF_FALSE, 0});
    size_t patchIndex = c.cur->code.size() - 1;

    ws->body->NodeCompile(c);

    if (c.cur->code.size() - begLoop > UINT8_MAX)
        c.CompileError(ws->Loc(), "Too much code to loop over");

    c.cur->code.push_back({Opcode::LOOP, static_cast<uint8_t>(begLoop)});

    size_t jumpSize = c.cur->code.size() - patchIndex;

    if (jumpSize > UINT8_MAX)
        c.CompileError(ws->Loc(), "Too much code to jump over");

    c.cur->code[patchIndex].op = static_cast<uint8_t>(jumpSize - 1);
}

void NodeCompiler::CompileFuncDecl(FuncDecl *fd, Compiler &c)
{
    c.Functions.push_back(Function());
    c.cur = &c.Functions.back();
    c.cur->arity = fd->argtypes.size();

    c.Symbols.funcVarBegin = c.Symbols.vars.size();

    c.Symbols.AddFunc(fd->ret, fd->name, fd->argtypes, false);
    c.Symbols.depth++;

    for (size_t i = 0; i < fd->argtypes.size(); i++)
        c.Symbols.AddVar(fd->argtypes[i], fd->paramIdentifiers[i]);

    for (auto &stmt : fd->body)
        stmt->NodeCompile(c);

    c.ClearCurrentDepthWithPOPInst();
    c.Symbols.depth--;
    c.Symbols.funcVarBegin = 0;
    c.cur = &c.Functions[0];
}

void NodeCompiler::CompileReturn(Return *r, Compiler &c)
{
    if (r->retVal == nullptr)
        c.cur->code.push_back({Opcode::RETURN_VOID, 0});
    else
    {
        r->retVal->NodeCompile(c);
        c.cur->code.push_back({Opcode::RETURN, 0});
    }
}

void NodeCompiler::CompileStructDecl(StructDecl *sd, Compiler &c)
{
    StructID s;
    s.type = GetTypeNameMap()[sd->name];
    s.parent = sd->parent;

    for (size_t i = 0; i < sd->decls.size(); i++)
    {
        DeclaredVar *asDV = dynamic_cast<DeclaredVar *>(sd->decls[i].get());
        s.memberNames.push_back(asDV->name);
        s.memTypes.push_back(asDV->t);
        s.init.push_back(asDV->value);
        s.nameTypes[asDV->name] = asDV->t;
    }

    s.isNull = true;
    c.Symbols.AddStruct(s);
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
            func.isLibFunc = c.libfuncs.size() + 1;
            c.Symbols.AddFunc(func);

            if (c.Symbols.funcs.size() > UINT8_MAX)
                c.CompileError(is->Loc(), "Cannot import more than " + std::to_string(UINT8_MAX) + " library functions in total");

            c.libfuncs.emplace_back(LibraryFunctionDef(func.name, library, func.argtypes.size()));
        }
    }
    return;
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