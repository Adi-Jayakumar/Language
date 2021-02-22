#include "nodecompiler.h"

//-----------------EXPRESSIONS---------------------//

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    CompileConst copy = CompileConst(l->t, l->Loc().literal);
    c.cur->constants.push_back(copy);
    c.cur->code.push_back({Opcode::GET_C, static_cast<uint8_t>(c.cur->constants.size() - 1)});
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
    a->val->NodeCompile(c);
    size_t stackIndex = SIZE_MAX;

    VarReference *targetAsVr = dynamic_cast<VarReference *>(a->target.get());

    if (targetAsVr != nullptr)
    {
        if (!c.ResolveVariable(targetAsVr->name, stackIndex))
        {
            c.cur->code.push_back({Opcode::VAR_A, static_cast<uint8_t>(stackIndex)});
            c.cur->code.push_back({Opcode::POP, 0});
            c.cur->code.push_back({Opcode::GET_V, static_cast<uint8_t>(stackIndex)});
        }
        else
        {
            c.cur->code.push_back({Opcode::VAR_A_GLOBAL, static_cast<uint8_t>(stackIndex)});
            c.cur->code.push_back({Opcode::POP, 0});
            c.cur->code.push_back({Opcode::GET_V_GLOBAL, static_cast<uint8_t>(stackIndex)});
        }
        return;
    }

    // the value is pushed onto the stack in the handling of the ARR_SET instruction
    ArrayIndex *targetAsAi = dynamic_cast<ArrayIndex *>(a->target.get());
    size_t arrLoc;
    c.ResolveVariable(targetAsAi->name, arrLoc);

    targetAsAi->index->NodeCompile(c);
    c.cur->code.push_back({Opcode::GET_V, c.cur->vars[arrLoc].index});
    c.cur->code.push_back({Opcode::ARR_SET, 0});
}

void NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    size_t stackIndex = SIZE_MAX;
    if (!c.ResolveVariable(vr->name, stackIndex))
        c.cur->code.push_back({Opcode::GET_V, static_cast<uint8_t>(stackIndex)});
    else
        c.cur->code.push_back({Opcode::GET_V_GLOBAL, static_cast<uint8_t>(stackIndex)});
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{

    size_t index = c.ResolveFunction(fc->name);

    if (index > UINT8_MAX)
        c.CompileError(fc->Loc(), "Too many functions");

    if (fc->args.size() > UINT8_MAX)
        c.CompileError(fc->Loc(), "Functions can only have " + std::to_string(UINT8_MAX) + " arguments");

    for (std::shared_ptr<Expr> &e : fc->args)
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

    c.cur->code.push_back({Opcode::CALL_F, static_cast<uint8_t>(index + 1)});
}

void NodeCompiler::CompileArrayIndex(ArrayIndex *ai, Compiler &c)
{
    size_t arrLoc;
    c.ResolveVariable(ai->name, arrLoc);

    if (arrLoc > UINT8_MAX)
        c.CompileError(ai->Loc(), "Cannot have more than " + std::to_string(UINT8_MAX) + " variables");

    c.cur->code.push_back({Opcode::GET_V, static_cast<uint8_t>(arrLoc)});
    ai->index->NodeCompile(c);

    c.cur->code.push_back({Opcode::ARR_INDEX, 0});
}

void NodeCompiler::CompileInlineArray(InlineArray *ia, Compiler &c)
{
    if (ia->size > UINT8_MAX)
        c.CompileError(ia->Loc(), "Inline arrays' max size is " + std::to_string(UINT8_MAX));

    CompileConst arr = CompileConst(ia->size);
    c.cur->constants.push_back(arr);

    size_t arrStackLoc = c.cur->constants.size() - 1;

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

    c.cur->code.push_back({Opcode::ARR_D, static_cast<uint8_t>(ia->size)});
}

void NodeCompiler::CompileDynamicAllocArray(DynamicAllocArray *da, Compiler &c)
{
    da->size->NodeCompile(c);
    c.cur->code.push_back({Opcode::ARR_ALLOC, 0});
}

//------------------STATEMENTS---------------------//

void NodeCompiler::CompileExprStmt(ExprStmt *es, Compiler &c)
{
    es->exp->NodeCompile(c);

    FunctionCall *asFC = dynamic_cast<FunctionCall *>(es->exp.get());

    // Function calls need not always have a POP after due to void functions
    if (asFC == nullptr)
        c.cur->code.push_back({Opcode::POP, 0});

    else
    {
        size_t index = c.ResolveFunction(asFC->name);
        if (c.funcs[index].ret.type != 0)
            c.cur->code.push_back({Opcode::POP, 0});
    }
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    // compile the initialiser
    dv->value->NodeCompile(c);

    // add to the list of variables
    c.cur->vars.push_back({dv->name, c.cur->depth, static_cast<uint8_t>(c.cur->vars.size())});

    // check if it is a global variable
    if (!c.isFunc)
        c.cur->code.push_back({Opcode::VAR_D_GLOBAL, static_cast<uint8_t>(c.cur->vars.size() - 1)});
}

void NodeCompiler::CompileBlock(Block *b, Compiler &c)
{
    c.cur->depth++;

    for (std::shared_ptr<Stmt> &s : b->stmts)
    {
        try
        {
            s->NodeCompile(c);
        }
        catch (const std::exception &e)
        {
            c.hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }

    c.cur->CleanUpVariables();
    c.cur->depth--;
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
    if (fd->argtypes.size() > UINT8_MAX)
        c.CompileError(fd->Loc(), "Functions can only have " + std::to_string(UINT8_MAX) + " number of arguments");

    c.funcs.push_back({fd->name, fd->ret});
    size_t numVars = 0;

    for (size_t i = 0; i < fd->argtypes.size(); i++)
    {
        CTVarID arg;
        arg.name = fd->paramIdentifiers[i];
        arg.depth = c.cur->depth;
        arg.index = c.cur->vars.size();

        c.cur->vars.push_back(arg);
        // c.cur->code.push_back({Opcode::VAR_D, static_cast<uint8_t>(numVars), static_cast<uint8_t>(c.cur->vars.size() - 1)});
        numVars++;
    }

    for (auto &s : fd->body)
    {
        try
        {
            s->NodeCompile(c);
        }
        catch (const std::exception &e)
        {
            c.hadError = true;
            std::cerr << e.what() << std::endl;
        }
    }
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

void InlineArray::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileInlineArray(this, c);
}

void DynamicAllocArray::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileDynamicAllocArray(this, c);
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
    c.chunks.push_back(Chunk());
    c.cur = &c.chunks.back();
    c.isFunc = true;

    c.cur->arity = argtypes.size();

    NodeCompiler::CompileFuncDecl(this, c);

    c.cur->CleanUpVariables();
    c.isFunc = false;
    c.cur = &c.chunks[0];
}

void Return::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileReturn(this, c);
}