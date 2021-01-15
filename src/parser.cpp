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

Block* Parser::ParseBlock()
{
    Advance();
    depth++;
    Block* result = new Block(depth);
    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        if(cur.type == TokenID::OPEN_BRACE)
            result->stmts.push_back(ParseBlock());
        else
            result->stmts.push_back(Declaration());
    }

    if(cur.type == TokenID::CLOSE_BRACE)
        Advance();
    else
        ParseError(cur, "Need to close braces");

    depth--;
    return result;
}

// ----------------------DECLARATIONS----------------------- //

Stmt *Parser::Declaration()
{
    if (cur.type == TokenID::TYPENAME)
        return VarDeclaration();
    else
        return Statement();
}

Stmt *Parser::VarDeclaration()
{
    Check(TokenID::TYPENAME, "Expect type name at the beginning of a declaration");
    uint8_t type = TypeNameMap[cur.literal];

    Advance();
    std::string name = cur.literal;
    Token loc = cur;

    Advance();
    Expr *init = nullptr;

    if (cur.type == TokenID::EQ)
    {
        Advance();
        init = Expression();
    }

    Check(TokenID::SEMI, "Expect ';' after variable declaration");
    Advance();

    return new DeclaredVar(type, name, init, loc);
}

// ----------------------DECLARATIONS----------------------- //

Stmt *Parser::Statement()
{
    return ExpressionStatement();
}

Stmt *Parser::ExpressionStatement()
{
    Token loc = cur;
    Expr *exp = Expression();
    Check(TokenID::SEMI, "Missing ';'");
    Advance();
    return new ExprStmt(exp, loc);
}

Expr *Parser::Expression()
{
    return Assignment();
}

Expr *Parser::Assignment()
{
    Expr *exp = EqualityCheck();

    if (cur.type == TokenID::EQ)
    {
        Token loc = cur;
        Advance();
        Expr *val = Assignment();

        VarReference *v = dynamic_cast<VarReference *>(exp);
        if (v != nullptr)
            return new Assign(v, val, loc);
    }
    return exp;
}

Expr *Parser::EqualityCheck()
{
    Expr *left = Comparison();
    Token op = cur;

    while (cur.type == TokenID::EQ_EQ || cur.type == TokenID::BANG_EQ)
    {
        Advance();
        Expr *right = Comparison();
        left = new Binary(left, op, right);
        op = cur;
    }

    return left;
}

Expr *Parser::Comparison()
{
    Expr *left = Sum();
    Token op = cur;

    while (cur.type == TokenID::GT || cur.type == TokenID::LT || cur.type == TokenID::GEQ || cur.type == TokenID::LEQ)
    {
        Advance();
        Expr *right = Sum();
        left = new Binary(left, op, right);
        op = cur;
    }

    return left;
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

    if (cur.type == TokenID::MINUS || cur.type == TokenID::BANG)
    {
        Advance();
        Expr *right = new Unary(prev, UnaryOp());
        return right;
    }

    return LiteralNode();
}

Expr *Parser::LiteralNode()
{
    Expr *res = nullptr;
    if (IsLiteral(cur))
        res = new Literal(cur);
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
        return new VarReference(loc);
    }
    else
        ParseError(cur, "[PARSE ERROR]: Misplaced token on line: " + std::to_string(cur.line) + "\nToken: '" + cur.literal + "'");
    Advance();
    return res;
}
