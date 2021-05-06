#include "nodecompiler.h"

RuntimeType NodeCompiler::TypeDataToRuntimeType(const TypeData &t)
{
    if (t.isArray)
        return RuntimeType::ARRAY;

    switch (t.type)
    {
    case 1:
    {
        return RuntimeType::INT;
    }
    case 2:
    {
        return RuntimeType::DOUBLE;
    }
    case 3:
    {
        return RuntimeType::BOOL;
    }
    case 4:
    {
        return RuntimeType::STRING;
    }
    case 5:
    {
        return RuntimeType::CHAR;
    }
    case 6:
    {
        return RuntimeType::NULL_T;
    }
    default:
    {
        return RuntimeType::STRUCT;
    }
    }
}

//-----------------EXPRESSIONS---------------------//

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    RuntimeObject copy = RuntimeObject(TypeDataToRuntimeType(l->t), l->Loc().literal);
    c.cur->values.push_back(copy);
    c.cur->code.push_back({Opcode::GET_C, static_cast<uint8_t>(c.cur->values.size() - 1)});
}

void NodeCompiler::CompileUnary(Unary *u, Compiler &c)
{
    u->right->NodeCompile(c);
    c.cur->code.push_back({TokenToOpcode({false, 0}, u->op.type, u->right->GetType(), true), 1});
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
        c.cur->code.push_back({Opcode::VAR_A, static_cast<uint8_t>(varStackLoc)});
    }
}

void NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    uint8_t varStackLoc = static_cast<uint8_t>(c.Symbols.FindVarByName(vr->name));
    c.cur->code.push_back({Opcode::GET_V, varStackLoc});
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{
}

void NodeCompiler::CompileArrayIndex(ArrayIndex *ai, Compiler &c)
{
}

void NodeCompiler::CompileBracedInitialiser(BracedInitialiser *ia, Compiler &c)
{
    if (ia->size > UINT8_MAX)
        c.CompileError(ia->Loc(), "Inline arrays' max size is " + std::to_string(UINT8_MAX));

    RuntimeObject arr;

    if (ia->GetType().isArray)
        arr = RuntimeObject(RuntimeType::ARRAY, ia->size);
    else
    {
        arr = RuntimeObject(RuntimeType::STRUCT, ia->size);
        arr.as.arr.type = ia->GetType().type;
    }

    c.cur->values.push_back(arr);
    size_t arrStackLoc = c.cur->values.size() - 1;

    if (arrStackLoc > UINT8_MAX)
        c.CompileError(ia->Loc(), "Too many variables");

    c.cur->code.push_back({Opcode::GET_C, static_cast<uint8_t>(arrStackLoc)});

    for (auto &e : ia->init)
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
    if (ia->GetType().isArray)
        c.cur->code.push_back({Opcode::ARR_D, static_cast<uint8_t>(ia->size)});
    else
        c.cur->code.push_back({Opcode::STRUCT_D, static_cast<uint8_t>(ia->size)});
}

void NodeCompiler::CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c)
{
    da->size->NodeCompile(c);
    c.cur->code.push_back({Opcode::ARR_ALLOC, 0});
}

void NodeCompiler::CompileFieldAccess(FieldAccess *fa, Compiler &c)
{
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
    c.cur->code.push_back({Opcode::POP, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    if (c.Symbols.vars.size() > UINT8_MAX)
        c.CompileError(dv->Loc(), "Max number of variables in a chunk is " + std::to_string(UINT8_MAX));

    c.Symbols.AddVar(dv->t, dv->name);
    dv->value->NodeCompile(c);
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

    i->thenBranch->NodeCompile(c);

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

    i->elseBranch->NodeCompile(c);

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