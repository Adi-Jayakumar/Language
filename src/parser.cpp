#include "parser.h"
#include "ASTPrinter.h"

Parser::Parser(const std::string &fPath)
{
    lex = Lexer(fPath);
    cur = lex.NextToken();
}

void Parser::Advance()
{
    prev = cur;
    cur = lex.NextToken();
}

Expr *Parser::Sum()
{
    Expr *left = Product();
    Token op = cur;

    while (cur.type == TokenID::PLUS || cur.type == TokenID::MINUS)
    {
        Advance();
        Expr *right = Product();
        left = new Binary(left, op, right);
        op = cur;
    }

    return left;
}

Expr *Parser::Product()
{
    Expr *left = UnaryOp();
    Token op = cur;

    while (cur.type == TokenID::STAR || cur.type == TokenID::SLASH)
    {
        Advance();
        Expr *right = UnaryOp();
        left = new Binary(left, op, right);
        op = cur;
    }

    return left;
}

Expr *Parser::UnaryOp()
{
    Expr *right = LiteralNode();
    Token op = cur;

    if (op.type == TokenID::MINUS)
    {
        right = new Unary(op, UnaryOp());
        op = cur;
    }

    return right;
}

Expr *Parser::LiteralNode()
{
    Expr *res = nullptr;
    if (cur.type == TokenID::DOUBLE_L)
        res = new Literal(stod(cur.literal));
    else if (cur.type == TokenID::OPEN_PAR)
    {
        Advance();
        res = Sum();
        
        // just in place of actual error handling
        if(cur.type != TokenID::CLOSE_PAR)
            std::cout << "NEED TO CLOSE THE PARENS" << std::endl;
    }
    Advance();
    return res;
}