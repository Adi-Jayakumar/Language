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

void NodeCompiler::CompileLiteral(Literal *l, Compiler &c)
{
    TypeID type = l->GetType().type;
    std::string literal = l->Loc().literal;
    Function *cur = c.cur;

    if (type == 1)
    {
        cur->ints.push_back(std::stoi(literal));
        if (cur->ints.size() > MAX_OPCODE)
            c.CompileError(l->Loc(), "Max number of int constants is " + std::to_string(MAX_OPCODE));

        cur->code.push_back({Opcode::LOAD_INT, static_cast<oprand_t>(cur->ints.size() - 1)});
    }
    else if (type == 2)
    {
        cur->doubles.push_back(std::stod(literal));
        if (cur->doubles.size() > MAX_OPCODE)
            c.CompileError(l->Loc(), "Max number of double constants is " + std::to_string(MAX_OPCODE));

        cur->code.push_back({Opcode::LOAD_DOUBLE, static_cast<oprand_t>(cur->doubles.size() - 1)});
    }
    else if (type == 3)
    {
        cur->bools.push_back(literal == "true" ? true : false);
        if (cur->bools.size() > MAX_OPCODE)
            c.CompileError(l->Loc(), "Max number of bool constants is " + std::to_string(MAX_OPCODE));

        cur->code.push_back({Opcode::LOAD_BOOL, static_cast<oprand_t>(cur->bools.size() - 1)});
    }
    else if (type == 4)
    {
        cur->strings.push_back(literal);
        if (cur->strings.size() > MAX_OPCODE)
            c.CompileError(l->Loc(), "Max number of string constants is " + std::to_string(MAX_OPCODE));

        cur->code.push_back({Opcode::LOAD_STRING, static_cast<oprand_t>(cur->strings.size() - 1)});
    }
    else if (type == 5)
    {
        cur->chars.push_back(literal[0]);
        if (cur->chars.size() > MAX_OPCODE)
            c.CompileError(l->Loc(), "Max number of char constants is " + std::to_string(MAX_OPCODE));

        cur->code.push_back({Opcode::LOAD_CHAR, static_cast<oprand_t>(cur->chars.size() - 1)});
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
    NodeCompiler::CompilerTryCatch(this, c);
}