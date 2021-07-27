#include "verifier.h"

void Verifier::VerificationError(std::string msg)
{
    Error e("[VERIFICATION ERROR] " + msg);
    throw e;
}

void Verifier::GenerateStrongestPost(std::vector<std::shared_ptr<Expr>> &pre)
{
    for (auto &p : pre)
    {
        if (dynamic_cast<VarReference *>(p.get()) != nullptr)
            stack.push_back(p);
        else if (dynamic_cast<Assign *>(p.get()) != nullptr)
            stack.push_back(dynamic_cast<Assign *>(p.get())->target); // TODO - Need to checkt that LHS = appropriate 'VarReference'
        else
            VerificationError("Inappropriate pre condition type");
    }

    for (size_t i = 0; i < f.code.size(); i++)
    {
        Op o = f.code[i];

        switch (o.code)
        {
        case Opcode::POP:
        {
            stack.pop_back();
            break;
        }
        case Opcode::LOAD_INT:
        {
            stack.push_back(std::make_shared<Literal>(f.ints[o.op]));
            break;
        }
        case Opcode::LOAD_DOUBLE:
        {
            stack.push_back(std::make_shared<Literal>(f.doubles[o.op]));
            break;
        }
        case Opcode::LOAD_BOOL:
        {
            stack.push_back(std::make_shared<Literal>(f.bools[o.op]));
            break;
        }
        case Opcode::LOAD_STRING:
        {
            stack.push_back(std::make_shared<Literal>(f.strings[o.op]));
            break;
        }
        case Opcode::LOAD_CHAR:
        {
            stack.push_back(std::make_shared<Literal>(f.chars[o.op]));
            break;
        }
        case Opcode::VAR_A:
        {
            stack[o.op] = stack.back();
            break;
        }
        case Opcode::GET_V:
        {
            stack.push_back(stack[o.op]);
            break;
        }
        case Opcode::JUMP_IF_FALSE:
        {
            // do checks
            break;
        }
        case Opcode::JUMP:
        {
            // do checks;
            break;
        }
        case Opcode::SET_IP:
        {
            // do checks
            break;
        }
        case Opcode::RETURN:
        {
            // do appropriate thing
            break;
        }
        case Opcode::I_ADD:
        case Opcode::DI_ADD:
        case Opcode::ID_ADD:
        case Opcode::D_ADD:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::PLUS, "+", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_SUB:
        case Opcode::D_SUB:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            Token op(TokenID::MINUS, "-", 0);

            if (o.op == 1)
            {
                stack.push_back(std::make_shared<Unary>(op, right));
                break;
            }

            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            stack.push_back(std::make_shared<Binary>(left, op, right));
        }
        case Opcode::DI_SUB:
        case Opcode::ID_SUB:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::MINUS, "-", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_MUL:
        case Opcode::DI_MUL:
        case Opcode::ID_MUL:
        case Opcode::D_MUL:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::STAR, "*", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_DIV:
        case Opcode::DI_DIV:
        case Opcode::ID_DIV:
        case Opcode::D_DIV:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::SLASH, "/", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_GT:
        case Opcode::DI_GT:
        case Opcode::ID_GT:
        case Opcode::D_GT:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::GT, ">", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_LT:
        case Opcode::DI_LT:
        case Opcode::ID_LT:
        case Opcode::D_LT:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::LT, "<", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_GEQ:
        case Opcode::DI_GEQ:
        case Opcode::ID_GEQ:
        case Opcode::D_GEQ:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::GEQ, ">=", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_LEQ:
        case Opcode::DI_LEQ:
        case Opcode::ID_LEQ:
        case Opcode::D_LEQ:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::LEQ, "<=", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_EQ_EQ:
        case Opcode::DI_EQ_EQ:
        case Opcode::ID_EQ_EQ:
        case Opcode::D_EQ_EQ:
        case Opcode::B_EQ_EQ:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::LEQ, "==", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::I_BANG_EQ:
        case Opcode::DI_BANG_EQ:
        case Opcode::ID_BANG_EQ:
        case Opcode::D_BANG_EQ:
        case Opcode::B_BANG_EQ:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::BANG_EQ, "!=", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::BANG:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();

            Token op(TokenID::BANG, "!", 0);
            stack.push_back(std::make_shared<Expr>(op, right));
            break;
        }
        default:
        {
            VerificationError("Cannot generate post condition using opcode " + ToString(o.code));
            break;
        }
        }
    }
}