#include "parser.h"

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
    Expr *left = LiteralNode();

    Advance();

    Token op = cur;

    while (cur.type == TokenID::PLUS || cur.type == TokenID::MINUS)
    {
        Advance();
        Expr *right = LiteralNode();
        left = new Binary(left, op, right);
        Advance();
        op = cur;
    }

    return left;
}

Expr *Parser::LiteralNode()
{
    if (cur.type == TokenID::DOUBLE_L)
        return new Literal(stod(cur.literal));

    return nullptr;
}