#include "nodecompiler.h"

/*
    TODO
        Compile uninitialised variables
*/

//-----------------EXPRESSIONS---------------------//

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    // c.cur->values.push_back(CreateRTOFromString(TypeDataToRuntimeType(l->t), l->Loc().literal.c_str()));
    // c.cur->code.push_back({Opcode::GET_C, static_cast<uint8_t>(c.cur->values.size() - 1)});
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
        c.cur->code.push_back({Opcode::CALL_F, static_cast<uint8_t>(1 + index)});
    else
    {
        size_t natFunc = c.Symbols.FindNativeFunctions(argtypes);
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

    TypeData voidType = {0, false};
    if (asFC == nullptr || asFC->GetType() != voidType)
        c.cur->code.push_back({Opcode::POP, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    if (c.Symbols.vars.size() > UINT8_MAX)
        c.CompileError(dv->Loc(), "Max number of variables in a chunk is " + std::to_string(UINT8_MAX));

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
    c.chunks.push_back(Chunk());
    c.cur = &c.chunks.back();
    c.cur->arity = fd->argtypes.size();

    c.Symbols.funcVarBegin = c.Symbols.vars.size();

    c.Symbols.AddFunc(fd->ret, fd->name, fd->argtypes);
    c.Symbols.depth++;

    for (size_t i = 0; i < fd->argtypes.size(); i++)
        c.Symbols.AddVar(fd->argtypes[i], fd->paramIdentifiers[i]);

    for (auto &stmt : fd->body)
        stmt->NodeCompile(c);

    c.ClearCurrentDepthWithPOPInst();
    c.Symbols.depth--;
    c.Symbols.funcVarBegin = 0;
    c.cur = &c.chunks[0];
}

void NodeCompiler::CompileReturn(Return *r, Compiler &c)
{
    if (r->retVal == nullptr)
        // the 1 in op's position is to ensure that we do not pop a value off the stack
        c.cur->code.push_back({Opcode::RETURN, 1});
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