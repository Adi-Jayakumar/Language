#include "parser.h"
#include "ASTPrinter.h"

Parser::Parser(const std::string &fPath)
{
    lex = Lexer(fPath);
    cur = lex.NextToken();
    depth = 0;
}

void Parser::ParseError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + "\n" + err);
    e.Dump();
}

void Parser::Check(TokenID t, std::string err)
{
    if (t != cur.type)
    {
        Error e = Error("[PARSE ERROR] On line " + std::to_string(cur.line) + "\n" + err);
        e.Dump();
    }
}

void Parser::Advance()
{
    prev = cur;
    cur = lex.NextToken();
}

std::shared_ptr<Block> Parser::ParseBlock()
{
    Advance();
    depth++;
    std::shared_ptr<Block> result = std::make_shared<Block>(depth);
    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        if (cur.type == TokenID::OPEN_BRACE)
            result->stmts.push_back(ParseBlock());
        else
            result->stmts.push_back(Declaration());
    }

    if (cur.type == TokenID::CLOSE_BRACE)
        Advance();
    else
        ParseError(cur, "Need to close braces");

    depth--;
    return result;
}

// ----------------------DECLARATIONS----------------------- //

std::shared_ptr<Stmt> Parser::Declaration()
{
    if (cur.type == TokenID::TYPENAME)
        return VarDeclaration();
    else
        return Statement();
}

std::shared_ptr<Stmt> Parser::VarDeclaration()
{
    Check(TokenID::TYPENAME, "Expect type name at the beginning of a declaration");
    uint8_t type = TypeNameMap[cur.literal];

    Advance();
    std::string name = cur.literal;
    Token loc = cur;

    Advance();
    std::shared_ptr<Expr> init = nullptr;

    if (cur.type == TokenID::EQ)
    {
        Advance();
        init = Expression();
    }

    Check(TokenID::SEMI, "Expect ';' after variable declaration");
    Advance();

    return std::make_shared<DeclaredVar>(type, name, init, loc);
}

// ----------------------DECLARATIONS----------------------- //

std::shared_ptr<Stmt> Parser::Statement()
{
    return ExpressionStatement();
}

std::shared_ptr<Stmt> Parser::ExpressionStatement()
{
    Token loc = cur;
    std::shared_ptr<Expr> exp = Expression();
    Check(TokenID::SEMI, "Missing ';'");
    Advance();
    return std::make_shared<ExprStmt>(exp, loc);
}

std::shared_ptr<Expr> Parser::Expression()
{
    return Assignment();
}

std::shared_ptr<Expr> Parser::Assignment()
{
    std::shared_ptr<Expr> exp = EqualityCheck();

    if (cur.type == TokenID::EQ)
    {
        Token loc = cur;
        Advance();
        std::shared_ptr<Expr> val = Assignment();

        VarReference *v = dynamic_cast<VarReference *>(exp.get());
        if (v != nullptr)
        {
            std::shared_ptr<VarReference> u = std::make_shared<VarReference>(v->loc);
            return std::make_shared<Assign>(u, val, loc);
        }
    }
    return exp;
}

std::shared_ptr<Expr> Parser::EqualityCheck()
{
    std::shared_ptr<Expr> left = Comparison();
    Token op = cur;

    while (cur.type == TokenID::EQ_EQ || cur.type == TokenID::BANG_EQ)
    {
        Advance();
        std::shared_ptr<Expr> right = Comparison();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

std::shared_ptr<Expr> Parser::Comparison()
{
    std::shared_ptr<Expr> left = Sum();
    Token op = cur;

    while (cur.type == TokenID::GT || cur.type == TokenID::LT || cur.type == TokenID::GEQ || cur.type == TokenID::LEQ)
    {
        Advance();
        std::shared_ptr<Expr> right = Sum();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

std::shared_ptr<Expr> Parser::Sum()
{
    std::shared_ptr<Expr> left = Product();
    Token op = cur;

    while (cur.type == TokenID::PLUS || cur.type == TokenID::MINUS)
    {
        Advance();
        std::shared_ptr<Expr> right = Product();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

std::shared_ptr<Expr> Parser::Product()
{
    std::shared_ptr<Expr> left = UnaryOp();
    Token op = cur;

    while (cur.type == TokenID::STAR || cur.type == TokenID::SLASH)
    {
        Advance();
        std::shared_ptr<Expr> right = UnaryOp();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

std::shared_ptr<Expr> Parser::UnaryOp()
{

    if (cur.type == TokenID::MINUS || cur.type == TokenID::BANG)
    {
        Advance();
        std::shared_ptr<Expr> right = std::make_shared<Unary>(prev, UnaryOp());
        return right;
    }

    return LiteralNode();
}

std::shared_ptr<Expr> Parser::LiteralNode()
{
    std::shared_ptr<Expr> res = nullptr;
    if (IsLiteral(cur))
        res = std::make_shared<Literal>(cur);
    else if (cur.type == TokenID::OPEN_PAR)
    {
        Advance();
        res = Expression();

        // just in place of actual error handling
        if (cur.type != TokenID::CLOSE_PAR)
            ParseError(cur, "NEED TO CLOSE THE PARENS");
    }
    else if (cur.type == TokenID::IDEN)
    {
        Token loc = cur;
        Advance();
        return std::make_shared<VarReference>(loc);
    }
    else
        ParseError(cur, "[PARSE ERROR]: Misplaced token on line: " + std::to_string(cur.line) + "\nToken: '" + cur.literal + "'");
    Advance();
    return res;
}
