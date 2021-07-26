#include "literalevaluator.h"

std::shared_ptr<Literal> BINARY_PLUS(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::INT_L;
            result.literal = std::to_string(l + r);
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l + r));
        }
    }
    else if (leftT == DOUBLE_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l + r));
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(l + r);
        }
    }
    else
    {
        result.type = TokenID::STRING_L;
        result.literal = leftL + rightL;
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_MINUS(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::INT_L;
            result.literal = std::to_string(l - r);
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l - r));
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l - r));
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(l - r);
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_STAR(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::INT_L;
            result.literal = std::to_string(l * r);
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l * r));
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l * r));
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(l * r);
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_SLASH(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::INT_L;
            result.literal = std::to_string(l / r);
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l / r));
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(static_cast<double>(l / r));
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::DOUBLE_L;
            result.literal = std::to_string(l / r);
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_GT(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l > r ? "true" : "false";
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l > r ? "true" : "false";
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l > r ? "true" : "false";
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l > r ? "true" : "false";
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_LT(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l < r ? "true" : "false";
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l < r ? "true" : "false";
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l < r ? "true" : "false";
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l < r ? "true" : "false";
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_GEQ(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l >= r ? "true" : "false";
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l >= r ? "true" : "false";
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l >= r ? "true" : "false";
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l >= r ? "true" : "false";
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_LEQ(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l <= r ? "true" : "false";
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l <= r ? "true" : "false";
        }
    }
    else
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l <= r ? "true" : "false";
        }
        else
        {
            double l = std::stod(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l <= r ? "true" : "false";
        }
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_EQ_EQ(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l == r ? "true" : "false";
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l == r ? "true" : "false";
        }
    }
    else if (leftT == DOUBLE_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l == r ? "true" : "false";
        }
        else
        {
            double l = leftL == "true" ? true : false;
            double r = rightL == "true" ? true : false;
            result.type = TokenID::BOOL_L;
            result.literal = l == r ? "true" : "false";
        }
    }
    else
    {
        bool l = leftL == "true" ? true : false;
        bool r = rightL == "true" ? true : false;
        result.type = TokenID::BOOL_L;
        result.literal = l == r ? "true" : "false";
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> BINARY_BANG_EQ(Literal *left, Literal *right)
{
    TypeData leftT = left->GetType();
    TypeData rightT = right->GetType();

    std::string leftL = left->Loc().literal;
    std::string rightL = right->Loc().literal;

    Token result = left->Loc();

    if (leftT == INT_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            int l = std::stoi(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l != r ? "true" : "false";
        }
        else
        {
            int l = std::stoi(leftL);
            double r = std::stod(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l != r ? "true" : "false";
        }
    }
    else if (leftT == DOUBLE_TYPE)
    {
        if (rightT == INT_TYPE)
        {
            double l = std::stod(leftL);
            int r = std::stoi(rightL);
            result.type = TokenID::BOOL_L;
            result.literal = l != r ? "true" : "false";
        }
        else
        {
            double l = leftL == "true" ? true : false;
            double r = rightL == "true" ? true : false;
            result.type = TokenID::BOOL_L;
            result.literal = l != r ? "true" : "false";
        }
    }
    else
    {
        bool l = leftL == "true" ? true : false;
        bool r = rightL == "true" ? true : false;
        result.type = TokenID::BOOL_L;
        result.literal = l != r ? "true" : "false";
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> UNARY_MINUS(Literal *right)
{
    TypeData rightT = right->GetType();
    std::string rightL = right->Loc().literal;
    Token result = right->Loc();

    if (rightT == INT_TYPE)
    {
        int i = std::stoi(rightL);
        result.type = TokenID::INT_L;
        result.literal = std::to_string(-i);
    }
    else
    {
        double d = std::stod(rightL);
        result.type = TokenID::DOUBLE_L;
        result.literal = std::to_string(-d);
    }
    return std::make_shared<Literal>(result);
}

std::shared_ptr<Literal> UNARY_BANG(Literal *right)
{
    std::string rightL = right->Loc().literal;
    Token result = right->Loc();

    bool b = rightL == "true" ? true : false;
    result.type = TokenID::BOOL_L;
    result.literal = b ? "true" : "false";
    return std::make_shared<Literal>(result);
}