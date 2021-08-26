#include "verifier.h"

void Verifier::VerificationError(std::string msg)
{
    Error e("[VERIFICATION ERROR] " + msg);
    throw e;
}

std::string ToString(ConditionOperation c)
{
    switch (c)
    {
    case ConditionOperation::POP:
    {
        return "POP";
    }
    case ConditionOperation::NEGATE_TOP:
    {
        return "PUSH_NEGATE_TOP";
    }
    }
    return "";
}

std::ostream &operator<<(std::ostream &out, ConditionOperation c)
{
    out << ToString(c);
    return out;
}

std::vector<std::shared_ptr<Expr>> Verifier::GetAssumptions()
{
    std::vector<std::shared_ptr<Expr>> res;
    for (auto &h : conditions)
        res.push_back(h);
    return res;
}

std::shared_ptr<Expr> GetResult(std::shared_ptr<Expr> returnVal)
{
    std::shared_ptr<VarReference> result = std::make_shared<VarReference>(Token(TokenID::IDEN, "result", 0));
    return std::make_shared<Binary>(result, Token(TokenID::EQ_EQ, "==", 0), returnVal);
}

void Verifier::GenerateStrongestPost(std::vector<std::shared_ptr<Expr>> &pre)
{
    if (pre.size() != f.arity)
        VerificationError("Trying to verify a function with an inappropriate number of preconditions");

    for (auto &p : pre)
    {
        if (dynamic_cast<VarReference *>(p.get()) != nullptr)
            stack.push_back(p);
        else if (dynamic_cast<Assign *>(p.get()) != nullptr)
            stack.push_back(dynamic_cast<Assign *>(p.get())->val); // TODO - Need to checkt that LHS = appropriate 'VarReference'
        else
            VerificationError("Inappropriate pre condition type");
    }

    size_t rp = 0;

    for (size_t i = 0; i < f.routines[0].size(); i++)
    {
        Op o = f.routines[rp][i];

        switch (o.code)
        {
        case Opcode::POP:
        {
            if (stack.size() == 1)
                stack.clear();
            else
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
        case Opcode::SET_IP:
        {
            // do checks
            VerificationError("There is a while loop in this function for which the strongest post condition cannot be calculated");
            break;
        }
        case Opcode::RETURN:
        {
            std::shared_ptr<Expr> retVal = stack.back();
            stack.pop_back();
            std::vector<std::shared_ptr<Expr>> assumptions = GetAssumptions();

            assumptions.push_back(GetResult(retVal));
            post.push_back(assumptions);
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
            break;
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
        case Opcode::B_AND_AND:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::AND_AND, "&&", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::B_OR_OR:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();
            std::shared_ptr<Expr> left = stack.back();
            stack.pop_back();
            Token op(TokenID::OR_OR, "||", 0);
            stack.push_back(std::make_shared<Binary>(left, op, right));
            break;
        }
        case Opcode::BANG:
        {
            std::shared_ptr<Expr> right = stack.back();
            stack.pop_back();

            Token op(TokenID::BANG, "!", 0);
            stack.push_back(std::make_shared<Unary>(op, right));
            break;
        }
        default:
        {
            VerificationError("Cannot generate post condition using opcode " + ToString(o.code));
            break;
        }
        }

        if (!conditionPop.empty())
        {
            ConditionStackOp cso = conditionPop.back();
            if (i == cso.idx)
            {
                switch (cso.op)
                {
                case ConditionOperation::POP:
                {
                    conditions.pop_back();
                    break;
                }
                case ConditionOperation::NEGATE_TOP:
                {
                    std::shared_ptr<Expr> condition = conditions.back();
                    conditions.pop_back();
                    Token bang(TokenID::BANG, "!", 0);
                    std::shared_ptr<Expr> negated = std::make_shared<Unary>(bang, condition);
                    conditions.push_back(negated);
                    break;
                }
                }
                conditionPop.pop_back();
            }
        }
    }
}