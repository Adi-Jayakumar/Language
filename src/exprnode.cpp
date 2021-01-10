#include "exprnode.h"

Expr::~Expr(){}

Literal::Literal(Token val)
{
    switch(DefaultTypeMap.at(val.type))
    {
        case 1:
        {
            typeID = 1;
            as.i = std::stoi(val.literal);
            break;
        }
        case 2:
        {
            typeID = 2;
            as.d = std::stod(val.literal);
            break;
        }
        case 3:
        {
            typeID = 3;
            if(val.literal == "true")
                as.b = true;
            else
                as.b = false;
        }
    }
}

Unary::Unary(Token _op, Expr* _right)
{
    op = _op;
    right = _right;
}

Unary::~Unary()
{
    delete right;
}

Binary::Binary(Expr* _left, Token _op, Expr* _right)
{
    left = _left;
    op = _op;
    right = _right;
}

Binary::~Binary()
{
    delete left;
    delete right;
}

Assign::Assign(std::string _name, Expr* _val)
{
    name = _name;
    val = _val;
}

Assign::~Assign()
{
    delete val;
}

VarReference::VarReference(std::string _name)
{
    name = _name;
}