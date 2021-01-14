#include "compiler.h"

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
        return "UNRECOGNISED OPCODE";
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
}

Chunk::~Chunk()
{
}

void Chunk::PrintCode()
{
    for (Op &o : code)
    {
        std::cout << ToString(o.code);
        if (o.code == Opcode::GET_C)
            std::cout << " " << constants[o.operand];
        std::cout << std::endl;
    }
}

void Chunk::CompileLiteral(Literal *l)
{
    CompileConst copy = CompileConst(l->typeID, l->loc.literal);
    constants.push_back(copy);
    code.push_back({Opcode::GET_C, static_cast<uint8_t>(constants.size() - 1)});
}

void Chunk::CompileUnary(Unary *u)
{
    u->right->NodeCompile(*this);
    code.push_back({TokenToOpcode(u->op.type), 0});
}

void Chunk::CompileBinary(Binary *b)
{
    b->left->NodeCompile(*this);
    b->right->NodeCompile(*this);
    code.push_back({TokenToOpcode(b->op.type), 0});
}

void Chunk::CompileAssign(Assign *a)
{
}

void Chunk::CompileVarReference(VarReference *vr)
{
}

void Chunk::CompileExprStmt(ExprStmt *es)
{
    es->exp->NodeCompile(*this);
    code.push_back({Opcode::POP, 0});
}
void Chunk::CompileDeclaredVar(DeclaredVar *dv) {}

void Chunk::CompileBlock(Block *b)
{
    for (Stmt *s : b->stmts)
        s->NodeCompile(*this);
}

void Literal::NodeCompile(Chunk &c)
{
    c.CompileLiteral(this);
}

void Unary::NodeCompile(Chunk &c)
{
    c.CompileUnary(this);
}

void Binary::NodeCompile(Chunk &c)
{
    c.CompileBinary(this);
}

void Assign::NodeCompile(Chunk &c)
{
    c.CompileAssign(this);
}

void VarReference::NodeCompile(Chunk &c)
{
    c.CompileVarReference(this);
}

void ExprStmt::NodeCompile(Chunk &c)
{
    c.CompileExprStmt(this);
}

void DeclaredVar::NodeCompile(Chunk &c)
{
    c.CompileDeclaredVar(this);
}

void Block::NodeCompile(Chunk &c)
{
    c.CompileBlock(this);
}