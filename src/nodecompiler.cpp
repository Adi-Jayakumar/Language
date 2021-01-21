#include "nodecompiler.h"

std::string ToString(Opcode o)
{
    switch (o)
    {
    case Opcode::POP:
    {
        return "POP";
    }
    case Opcode::GET_C:
    {
        return "GET_C";
    }
    case Opcode::GET_V:
    {
        return "GET_V";
    }
    case Opcode::DEL_V:
    {
        return "DEL_V";
    }
    case Opcode::VAR_D:
    {
        return "VAR_D";
    }
    case Opcode::VAR_A:
    {
        return "VAR_A";
    }
    case Opcode::JUMP_IF_FALSE:
    {
        return "JUMP_IF_FALSE";
    }
    case Opcode::JUMP:
    {
        return "JUMP";
    }
    case Opcode::ADD:
    {
        return "ADD";
    }
    case Opcode::SUB:
    {
        return "SUB";
    }
    case Opcode::MUL:
    {
        return "MUL";
    }
    case Opcode::DIV:
    {
        return "DIV";
    }

    case Opcode::GT:
    {
        return "GT";
    }
    case Opcode::LT:
    {
        return "LT";
    }
    case Opcode::GEQ:
    {
        return "GEQ";
    }
    case Opcode::LEQ:
    {
        return "LEQ";
    }

    case Opcode::EQ_EQ:
    {
        return "EQ_EQ";
    }
    case Opcode::BANG_EQ:
    {
        return "BANG_EQ";
    }
    default:
    {
        return "UNRECOGNISED OPCODE " + std::to_string((uint16_t)o);
    }
    }
}

Opcode TokenToOpcode(TokenID t)
{
    if (t == TokenID::PLUS)
        return Opcode::ADD;
    else if (t == TokenID::MINUS)
        return Opcode::SUB;
    else if (t == TokenID::STAR)
        return Opcode::MUL;
    else if (t == TokenID::SLASH)
        return Opcode::DIV;
    else if (t == TokenID::GT)
        return Opcode::GT;
    else if (t == TokenID::LT)
        return Opcode::LT;
    else if (t == TokenID::GEQ)
        return Opcode::GEQ;
    else if (t == TokenID::LEQ)
        return Opcode::LEQ;
    else if (t == TokenID::EQ_EQ)
        return Opcode::EQ_EQ;
    else if (t == TokenID::BANG_EQ)
        return Opcode::BANG_EQ;
    else
        return Opcode::NONE;
}

void Chunk::PrintCode()
{
    for (DebugOp &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::GET_C)
            std::cout << " '" << constants[o.operand] << "' at index: " << +o.operand;
        else if (o.code == Opcode::GET_V || o.code == Opcode::VAR_D || o.code == Opcode::VAR_A)
            std::cout << " '" << vars[o.debug].name << "' at runtime index: " << +o.operand << " at compile time index: " << o.debug;
        else
            std::cout << " " << +o.operand;

        std::cout << std::endl;
    }
}

size_t Chunk::ResolveVariable(std::string &name)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if ((vars[i].name.length() == name.length()) && (vars[i].name == name))
            return i;
    }
    return 255;
}

void Chunk::CleanUpVariables()
{
    // while (!vars.empty() && vars.back().depth == depth)
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].depth == depth)
        {
            numPops++;
            code.push_back({Opcode::POP, 0, 0});
            code.push_back({Opcode::DEL_V, 0, 0});
        }
    }
}

void NodeCompiler::CompileError(std::string err)
{
    Error e = Error("[COMPILE ERROR] " + err);
    e.Dump();
}

//-----------------EXPRESSIONS---------------------//

void NodeCompiler::CompileLiteral(Literal *l, Chunk &c)
{
    CompileConst copy = CompileConst(l->typeID, l->Loc().literal);
    c.constants.push_back(copy);
    c.code.push_back({Opcode::GET_C, static_cast<uint16_t>(c.constants.size() - 1), 0});
}

void NodeCompiler::CompileUnary(Unary *u, Chunk &c)
{
    u->right->NodeCompile(c);
    c.code.push_back({TokenToOpcode(u->op.type), 0, 0});
}

void NodeCompiler::CompileBinary(Binary *b, Chunk &c)
{
    b->left->NodeCompile(c);
    b->right->NodeCompile(c);
    c.code.push_back({TokenToOpcode(b->op.type), 0, 0});
}

void NodeCompiler::CompileAssign(Assign *a, Chunk &c)
{
    size_t index = c.ResolveVariable(a->var->name);
    a->val->NodeCompile(c);
    c.code.push_back({Opcode::VAR_A, c.vars[index].index, index});
    c.code.push_back({Opcode::GET_V, c.vars[index].index, index});
    c.code.push_back({Opcode::POP, 0, 0});
}

void NodeCompiler::CompileVarReference(VarReference *vr, Chunk &c)
{
    size_t index = c.ResolveVariable(vr->name);
    c.code.push_back({Opcode::GET_V, c.vars[index].index, index});
}

//------------------STATEMENTS---------------------//

void NodeCompiler::CompileExprStmt(ExprStmt *es, Chunk &c)
{
    es->exp->NodeCompile(c);
    c.code.push_back({Opcode::POP, 0, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Chunk &c)
{
    c.vars.push_back({dv->name, c.depth, 0});
    uint16_t rtindex = static_cast<uint16_t>(c.vars.size() - 1 - c.numPops);
    c.vars.back().index = rtindex;
    dv->value->NodeCompile(c);
    c.code.push_back({Opcode::VAR_D, rtindex, static_cast<uint16_t>(c.vars.size() - 1)});
    // c.code.push_back({Opcode::POP, 0, 0});
}

void NodeCompiler::CompileBlock(Block *b, Chunk &c)
{
    c.depth++;
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s->NodeCompile(c);
    c.CleanUpVariables();
    c.depth--;
}

void NodeCompiler::CompileIfStmt(IfStmt *i, Chunk &c)
{
    i->cond->NodeCompile(c);
    c.code.push_back({Opcode::JUMP_IF_FALSE, 0, 0});

    size_t patchIndex = c.code.size() - 1;
    size_t befSize = c.code.size();

    i->thenBranch->NodeCompile(c);

    c.code.push_back({Opcode::POP, 0, 0});

    size_t sizeDiff = c.code.size() - befSize;

    if (sizeDiff > UINT16_MAX)
        CompileError("Too much code to junmp over");

    c.code[patchIndex].operand = static_cast<uint16_t>(sizeDiff);

    if (i->elseBranch == nullptr)
        return;
    c.code[patchIndex].operand++;
    c.code.push_back({Opcode::JUMP, 0, 0});
    patchIndex = c.code.size() - 1;
    befSize = c.code.size();

    i->elseBranch->NodeCompile(c);

    sizeDiff = c.code.size() - befSize;
    if (sizeDiff > UINT16_MAX)
        CompileError("Too much code to junmp over");
    c.code[patchIndex].operand = static_cast<uint16_t>(sizeDiff);
}

void NodeCompiler::CompileFuncDecl(FuncDecl *fd, Chunk &c)
{
    for (size_t i = 0; i < fd->params.size(); i++)
    {
        if (fd->params[i].type == TokenID::IDEN)
        {

            CTVarID arg;
            arg.name = fd->params[i].literal;
            arg.depth = c.depth;
            arg.index = c.vars.size();

            c.vars.push_back(arg);
        }
    }

    for (auto &s : fd->body)
    {
        s->NodeCompile(c);
    }
    c.CleanUpVariables();
}

void NodeCompiler::CompileReturn(Return *r, Chunk &c)
{
    return;
}

void NodeCompiler::CompileFunctionCall(FunctionCall *fc, Chunk &c)
{
    return;
}

//-----------------EXPRESSIONS---------------------//

void Literal::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileLiteral(this, c);
}

void Unary::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileUnary(this, c);
}

void Binary::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileBinary(this, c);
}

void Assign::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileAssign(this, c);
}

void VarReference::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileVarReference(this, c);
}

void FunctionCall::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileFunctionCall(this, c);
}

//------------------STATEMENTS---------------------//

void ExprStmt::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileExprStmt(this, c);
}

void DeclaredVar::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileDeclaredVar(this, c);
}

void Block::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileBlock(this, c);
}

void IfStmt::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileIfStmt(this, c);
}

void FuncDecl::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileFuncDecl(this, c);
}

void Return::NodeCompile(Chunk &c)
{
    NodeCompiler::CompileReturn(this, c);
}