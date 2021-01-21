#include "parser.h"
#include "ASTPrinter.h"

Parser::Parser(const std::string &fPath)
{
    lex = Lexer(fPath);
    cur = lex.NextToken();
    next = lex.NextToken();
    depth = 0;
}

void Parser::ParseError(Token loc, std::string err)
{
    Error e = Error("[PARSE ERROR] On line " + std::to_string(loc.line) + "\n" + err);
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
    cur = next;
    if (cur.type != TokenID::END)
        next = lex.NextToken();
}

std::shared_ptr<Block> Parser::ParseBlock()
{
    Advance();
    depth++;
    std::shared_ptr<Block> result = std::make_shared<Block>(depth, cur);
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
    else if (cur.type == TokenID::FUNC)
        return FuncDeclaration();
    else
        return Statement();
}

std::shared_ptr<Stmt> Parser::Statement()
{
    if (cur.type == TokenID::OPEN_BRACE)
        return ParseBlock();
    else if (cur.type == TokenID::IF)
        return IfStatement();
    return ExpressionStatement();
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

std::shared_ptr<Stmt> Parser::FuncDeclaration()
{
    if (depth > 1)
        Check(TokenID::END, "Cannot declare function inside nested scope");

    Token beg = cur;
    Advance();
    Check(TokenID::TYPENAME, "Expect a return type after function declaration");

    TypeID ret = TypeNameMap[cur.literal];

    Advance();
    Check(TokenID::IDEN, "Expect name after function declaration");

    std::string name = cur.literal;

    Advance();
    Check(TokenID::OPEN_PAR, "Expect argument list after function declaration");
    Advance();
    std::vector<Token> params;

    while (cur.type != TokenID::CLOSE_PAR)
    {
        if (cur.type != TokenID::COMMA)
            params.push_back(cur);
        Advance();
    }

    Advance();
    Check(TokenID::OPEN_BRACE, "Function body must start with an open brace");

    depth++;

    Advance();

    std::vector<std::shared_ptr<Stmt>> body;

    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        body.push_back(Statement());
    }

    Check(TokenID::CLOSE_BRACE, "Missing close brace");
    depth--;
    Advance();
    std::shared_ptr<FuncDecl> func = std::make_shared<FuncDecl>(ret, name, params, body, beg);
    return func;
}

// ----------------------STATEMENTS----------------------- //

std::shared_ptr<Stmt> Parser::IfStatement()
{
    Advance();
    Check(TokenID::OPEN_PAR, "Need an open paranthesis at the beginning of an if statement");
    Advance();
    std::shared_ptr<Expr> cond = Expression();
    Check(TokenID::CLOSE_PAR, "Missing a close parenthesis");
    Advance();
    std::shared_ptr<Stmt> thenBranch = Statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (cur.type == TokenID::ELSE)
    {
        Advance();
        elseBranch = Statement();
    }
    return std::make_shared<IfStmt>(cond, thenBranch, elseBranch, cur);
}

std::shared_ptr<Stmt> Parser::ExpressionStatement()
{
    Token loc = cur;

    std::shared_ptr<Expr> exp;

    if (cur.type == TokenID::IDEN && next.type == TokenID::OPEN_PAR)
        exp = FuncCall();
    else
        exp = Expression();

    Check(TokenID::SEMI, "Missing ';'");
    Advance();
    return std::make_shared<ExprStmt>(exp, loc);
}

// ----------------------EPRESSIONS----------------------- //

std::shared_ptr<Expr> Parser::Expression()
{
    return Assignment();
}

std::shared_ptr<Expr> Parser::FuncCall()
{
    std::string name = cur.literal;

    // skipping the name
    Advance();


    std::vector<std::shared_ptr<Expr>> args;

    while (cur.type != TokenID::CLOSE_PAR && cur.type != TokenID::END)
    {
        Advance();
        if (cur.type != TokenID::COMMA)
            args.push_back(Expression());
    }

    Check(TokenID::CLOSE_PAR, "Need to close parenthesis");
    Advance();

    return std::make_shared<FunctionCall>(name, args, cur);
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
            std::shared_ptr<VarReference> u = std::make_shared<VarReference>(v->Loc());
            return std::make_shared<Assign>(u, val, loc);
        }
        else
        {
            ParseError(cur, "Invalid assignment target");
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