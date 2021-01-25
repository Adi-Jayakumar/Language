#include "nodecompiler.h"

void NodeCompiler::CompileError(std::string err)
{
    Error e = Error("[COMPILE ERROR] " + err);
    e.Dump();
}

//-----------------EXPRESSIONS---------------------//

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    CompileConst copy = CompileConst(l->typeID, l->Loc().literal);
    c.cur->constants.push_back(copy);
    c.cur->code.push_back({Opcode::GET_C, static_cast<uint8_t>(c.cur->constants.size() - 1), 0});
}

void NodeCompiler::CompileUnary(Unary *u, Compiler &c)
{
    u->right->NodeCompile(c);
    c.cur->code.push_back({TokenToOpcode(u->op.type), 0, 0});
}

void NodeCompiler::CompileBinary(Binary *b, Compiler &c)
{
    b->left->NodeCompile(c);
    b->right->NodeCompile(c);
    c.cur->code.push_back({TokenToOpcode(b->op.type), 0, 0});
}

void NodeCompiler::CompileAssign(Assign *a, Compiler &c)
{
    size_t index = c.cur->ChunkResolveVariable(a->var->name);
    a->val->NodeCompile(c);
    c.cur->code.push_back({Opcode::VAR_A, c.cur->vars[index].index, static_cast<uint8_t>(index)});
    c.cur->code.push_back({Opcode::GET_V, c.cur->vars[index].index, static_cast<uint8_t>(index)});
    c.cur->code.push_back({Opcode::POP, 0, 0});
}

void NodeCompiler::CompileVarReference(VarReference *vr, Compiler &c)
{
    size_t index = c.cur->ChunkResolveVariable(vr->name);
    c.cur->code.push_back({Opcode::GET_V, c.cur->vars[index].index, static_cast<uint8_t>(index)});
}

//------------------STATEMENTS---------------------//

void NodeCompiler::CompileExprStmt(ExprStmt *es, Compiler &c)
{
    es->exp->NodeCompile(c);

    // TEMPORARY THING UNTIL WE SUPPORT RETURN STATEMENTS
    if (dynamic_cast<FunctionCall *>(es->exp.get()) == nullptr)
        c.cur->code.push_back({Opcode::POP, 0, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Compiler &c)
{
    c.cur->vars.push_back({dv->name, c.cur->depth, 0});
    uint8_t rtindex = static_cast<uint8_t>(c.cur->vars.size() - 1);
    c.cur->vars.back().index = rtindex;
    if (dv->value != nullptr)
        dv->value->NodeCompile(c);
    else
    {
        CompileConst defVal;
        switch (dv->tId)
        {
        case 1:
        {
            defVal = CompileConst(1, "0");
            break;
        }
        case 2:
        {
            defVal = CompileConst(2, "0");
            break;
        }
        case 3:
        {
            defVal = CompileConst(3, "false");
            break;
        }
        default:
        {
            defVal = CompileConst(0, "");
            break;
        }
        }

        c.cur->constants.push_back(defVal);
        c.cur->code.push_back({Opcode::GET_C, static_cast<uint8_t>(c.cur->constants.size() - 1), 0});
    }
    c.cur->code.push_back({Opcode::VAR_D, rtindex, static_cast<uint8_t>(c.cur->vars.size() - 1)});
    // c.cur->code.push_back({Opcode::POP, 0, 0});
}

void NodeCompiler::CompileBlock(Block *b, Compiler &c)
{
    c.cur->depth++;
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s->NodeCompile(c);
    c.cur->CleanUpVariables();
    c.cur->depth--;
}

void NodeCompiler::CompileIfStmt(IfStmt *i, Compiler &c)
{
    i->cond->NodeCompile(c);
    c.cur->code.push_back({Opcode::JUMP_IF_FALSE, 0, 0});

    size_t patchIndex = c.cur->code.size() - 1;
    size_t befSize = c.cur->code.size();

    i->thenBranch->NodeCompile(c);

    size_t sizeDiff = c.cur->code.size() - befSize;

    if (sizeDiff > UINT8_MAX)
        CompileError("Too much code to junmp over");

    c.cur->code[patchIndex].op1 = static_cast<uint8_t>(sizeDiff);

    if (i->elseBranch == nullptr)
        return;

    c.cur->code[patchIndex].op1++;

    c.cur->code.push_back({Opcode::JUMP, 0, 0});

    patchIndex = c.cur->code.size() - 1;
    befSize = c.cur->code.size();

    i->elseBranch->NodeCompile(c);

    sizeDiff = c.cur->code.size() - befSize;
    if (sizeDiff > UINT8_MAX)
        CompileError("Too much code to junmp over");
    c.cur->code[patchIndex].op1 = static_cast<uint8_t>(sizeDiff);
}

void NodeCompiler::CompileFuncDecl(FuncDecl *fd, Compiler &c)
{
    if (fd->params.size() > UINT8_MAX)
        CompileError("Functions can only have " + std::to_string(UINT8_MAX) + " number of arguments");

    c.funcs.push_back(fd->name);
    size_t numVars = 0;

    for (size_t i = 0; i < fd->params.size(); i++)
    {
        if (fd->params[i].type == TokenID::IDEN)
        {
            CTVarID arg;
            arg.name = fd->params[i].literal;
            arg.depth = c.cur->depth;
            arg.index = c.cur->vars.size();

            c.cur->vars.push_back(arg);
            c.cur->code.push_back({Opcode::VAR_D, static_cast<uint8_t>(numVars), static_cast<uint8_t>(c.cur->vars.size() - 1)});
            numVars++;
        }
    }

    for (auto &s : fd->body)
    {
        s->NodeCompile(c);
    }
}

void NodeCompiler::CompileReturn(Return *r, Compiler &c)
{
    if (r->retVal == nullptr)
        // the 1 in op1's position is to ensure that we do not pop a value off the stack
        c.cur->code.push_back({Opcode::RETURN, 1, 0});
    else
    {
        r->retVal->NodeCompile(c);
        c.cur->code.push_back({Opcode::RETURN, 0, 0});
    }
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Compiler &c)
{
    size_t index = c.ResolveFunction(fc->name);

    if (index > UINT8_MAX)
        CompileError("Too many functions");

    if (fc->args.size() > UINT8_MAX)
        CompileError("Functions can only have " + std::to_string(UINT8_MAX) + " number of arguments");

    for (std::shared_ptr<Expr> &e : fc->args)
        e->NodeCompile(c);

    c.cur->code.push_back({Opcode::CALL_F, static_cast<uint8_t>(index + 1), static_cast<uint8_t>(fc->args.size())});
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

void FuncDecl::NodeCompile(Compiler &c)
{
    c.chunks.push_back(Chunk());
    c.cur = &c.chunks.back();

    // c.isInFunc = true;
    // c.curArity = params.size();

    NodeCompiler::CompileFuncDecl(this, c);

    // c.isInFunc = false;
    // c.curArity = -1;

    c.cur->CleanUpVariables();

    c.cur = &c.chunks[0];
}

void Return::NodeCompile(Compiler &c)
{
    NodeCompiler::CompileReturn(this, c);
}