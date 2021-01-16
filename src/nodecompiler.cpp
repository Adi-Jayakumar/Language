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
    case Opcode::VAR_D:
    {
        return "VAR_D";
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
        return "UNRECOGNISED OPCODE " + std::to_string((uint8_t)o);
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

CompileConst::CompileConst(TypeID _type, std::string &literal)
{
    type = _type;
    switch (type)
    {
    case 1:
    {
        type = 1;
        as.i = std::stoi(literal);
        break;
    }
    case 2:
    {
        type = 2;
        as.d = std::stod(literal);
        break;
    }
    case 3:
    {
        type = 3;
        if (literal == "true")
            as.b = true;
        else
            as.b = false;
    }
    }
}

std::ostream &operator<<(std::ostream &out, CompileConst &cc)
{
    switch (cc.type)
    {
    case UINT8_MAX:
    {
        out << cc.as.i;
        break;
    }
    case 1:
    {
        out << cc.as.i;
        break;
    }
    case 2:
    {
        out << cc.as.d;
        break;
    }
    case 3:
    {
        if (cc.as.b)
            out << "true";
        else
            out << "false";
        break;
    }
    }
    return out;
}

void Chunk::PrintCode()
{
    for (Op &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::GET_C)
            std::cout << " " << constants[o.operand];
        else if (o.code == Opcode::GET_V || o.code == Opcode::VAR_D)
            std::cout << " " << vars[o.operand].name;
        else
            std::cout << " " << +o.operand;

        std::cout << std::endl;
    }
}

size_t Chunk::ResolveVariable(std::string &name, uint8_t depth)
{
    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if ((depth == vars[i].depth) && (vars[i].name.length() == name.length()) && (vars[i].name == name))
            return i;
    }
    return ~0;
}

void Chunk::CleanUpVariables()
{

    // std::cout << "Chunk depth: " << depth << std::endl;
    // for(int i = vars.size() - 1; i >= 0; i--)
    //     std::cout << "name: " << vars[i].name << " depth: " << +vars[i].depth << " val: " << vars[i].decl << std::endl;

    while(!vars.empty() && vars.back().depth == depth)
        vars.pop_back();
}

void NodeCompiler::CompileLiteral(Literal *l, Chunk &c)
{
    CompileConst copy = CompileConst(l->typeID, l->loc.literal);
    c.constants.push_back(copy);
    c.code.push_back({Opcode::GET_C, static_cast<uint8_t>(c.constants.size() - 1)});
}

void NodeCompiler::CompileUnary(Unary *u, Chunk &c)
{
    u->right->NodeCompile(c);
    c.code.push_back({TokenToOpcode(u->op.type), 0});
}

void NodeCompiler::CompileBinary(Binary *b, Chunk &c)
{
    b->left->NodeCompile(c);
    b->right->NodeCompile(c);
    c.code.push_back({TokenToOpcode(b->op.type), 0});
}

void NodeCompiler::CompileAssign(Assign *a, Chunk &c)
{
    size_t index = c.ResolveVariable(a->var->name, c.depth);
    a->val->NodeCompile(c);
    c.code.push_back({Opcode::VAR_D, static_cast<uint8_t>(index)});
    c.code.push_back({Opcode::GET_V, static_cast<uint8_t>(index)});
}

void NodeCompiler::CompileVarReference(VarReference *vr, Chunk &c)
{
    size_t index = c.ResolveVariable(vr->name, c.depth);
    c.code.push_back({Opcode::GET_V, static_cast<uint8_t>(index)});
}

void NodeCompiler::CompileExprStmt(ExprStmt *es, Chunk &c)
{
    es->exp->NodeCompile(c);
    c.code.push_back({Opcode::POP, 0});
}

void NodeCompiler::CompileDeclaredVar(DeclaredVar *dv, Chunk &c)
{
    c.vars.push_back({0, dv->name, c.depth});
    dv->value->NodeCompile(c);
    c.code.push_back({Opcode::VAR_D, static_cast<uint8_t>(c.vars.size() - 1)});
}

void NodeCompiler::CompileBlock(Block *b, Chunk &c)
{
    c.depth++;
    for (std::shared_ptr<Stmt> &s : b->stmts)
        s.get()->NodeCompile(c);
    c.CleanUpVariables();
    c.depth--;
}

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