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
    Error e = Error("[PARSE ERROR] On line " + std::to_string(loc.line) + "\n" + err + "\n");
    throw e;
}

void Parser::PanicMode(std::initializer_list<TokenID> recovery)
{
    while (true)
    {
        bool recovered = false;
        for (TokenID cand : recovery)
        {
            if (cand == cur.type)
            {
                recovered = true;
                break;
            }
        }
        if (!recovered)
            Advance();
        else
            break;
    }
}

void Parser::Check(TokenID t, std::string err)
{
    if (t != cur.type)
        ParseError(cur, err);
}

void Parser::Advance()
{
    prev = cur;
    cur = next;
    if (cur.type != TokenID::END)
        next = lex.NextToken();
}

TypeData Parser::ParseType(std::string err)
{
    if (cur.type == TokenID::TYPENAME)
    {
        std::string sType = cur.literal;
        Advance();
        return GetTypeNameMap()[sType];
    }
    else if (cur.type == TokenID::ARRAY)
    {
        Check(TokenID::ARRAY, "Types can only be a default type name or 'Array<T>'");
        Advance();

        Check(TokenID::LT, "The element type of an array is surrounded by angle brackets");
        Advance();

        int dim = 1;
        TypeData type;
        if (cur.type == TokenID::INT_L)
        {
            dim = std::stoi(cur.literal);
            if (dim < 1)
                ParseError(cur, "Multi dimensional array's dimension must be a positive int literal");
            Advance();

            Check(TokenID::COMMA, "Expect comma seperating dimension and type name");
            Advance();

            Check(TokenID::TYPENAME, "Expect type name in multi dimensional array type");
            type = GetTypeNameMap()[cur.literal];
            Advance();
        }
        else
        {
            Check(TokenID::TYPENAME, "1 dimensional array requires type name");
            type = GetTypeNameMap()[cur.literal];
            Advance();
        }

        Check(TokenID::GT, "Missing '>'");
        Advance();
        type.isArray = dim;
        return type;
    }
    ParseError(cur, err);
    // never reaches here, just to silence compiler warnings
    return {false, UINT8_MAX};
}

std::vector<std::shared_ptr<Stmt>> Parser::Parse()
{
    std::vector<std::shared_ptr<Stmt>> res;
    while (cur.type != TokenID::END)
    {
        try
        {
            res.push_back(Statement());
        }
        catch (const std::exception &e)
        {
            hadError = true;
            PanicMode({TokenID::SEMI, TokenID::END});
            Advance();
            std::cerr << e.what() << std::endl;
        }
    }
    return res;
}

std::shared_ptr<Stmt> Parser::Statement()
{
    if (cur.type == TokenID::OPEN_BRACE)
        return ParseBlock();
    else if (cur.type == TokenID::IF)
        return IfStatement();
    else if (cur.type == TokenID::WHILE)
        return WhileStatement();
    else if (cur.type == TokenID::RETURN)
    {
        // skipping the 'return' token
        Advance();

        // if the statement is 'return;'
        if (cur.type == TokenID::SEMI)
        {
            Advance();
            return std::make_shared<Return>(nullptr, cur);
        }

        // parsing the return value
        std::shared_ptr<Expr> retVal = Expression();

        // checking for the semicolon
        Check(TokenID::SEMI, "Require ';' at the end of a return statement");

        // advancing over the semicolon
        Advance();
        return std::make_shared<Return>(retVal, cur);
    }
    else if (cur.type == TokenID::IMPORT || cur.type == TokenID::FROM)
        return ParseImportStmt();
    return Declaration();
}

// ----------------------DECLARATIONS----------------------- //

std::shared_ptr<Stmt> Parser::Declaration()
{
    if (cur.type == TokenID::TYPENAME || cur.type == TokenID::ARRAY)
        return VarDeclaration();
    else if (cur.type == TokenID::FUNC)
        return FuncDeclaration();
    else if (cur.type == TokenID::STRUCT)
        return ParseStructDecl();
    else
        return ExpressionStatement();
}

std::shared_ptr<Stmt> Parser::VarDeclaration()
{
    TypeData type = ParseType("Expect type name at the beginning of a variable decaration");
    if (type.type == 0)
        ParseError(cur, "A variable cannot have 'void' type");

    Check(TokenID::IDEN, "Expect identifier after type name of a variable declaration");

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
        ParseError(cur, "Cannot declare function inside nested scope");

    Token beg = cur;
    Advance();
    TypeData ret = ParseType("Invalid return type");

    Check(TokenID::IDEN, "Expect name after function declaration");

    std::string name = cur.literal;

    Advance();
    Check(TokenID::OPEN_PAR, "Expect argument list after function declaration");
    Advance();

    std::vector<TypeData> argtypes;
    std::vector<std::string> paramIdentifiers;

    while (cur.type != TokenID::CLOSE_PAR && cur.type != TokenID::END)
    {
        try
        {
            argtypes.push_back(ParseType("Function arguments need types"));

            Check(TokenID::IDEN, "Arguments in function must have names");
            paramIdentifiers.push_back(cur.literal);

            Advance();
            if (cur.type == TokenID::COMMA)
                Advance();
        }
        catch (const std::exception &e)
        {
            hadError = true;
            PanicMode({TokenID::COMMA, TokenID::END});
            Advance();
            std::cerr << e.what() << std::endl;
        }
    }

    Advance();
    Check(TokenID::OPEN_BRACE, "Function body must start with an open brace");

    depth++;

    Advance();

    std::vector<std::shared_ptr<Stmt>> body;

    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        try
        {
            body.push_back(Statement());
        }
        catch (const std::exception &e)
        {
            hadError = true;
            PanicMode({TokenID::SEMI, TokenID::END});
            Advance();
            std::cerr << e.what() << std::endl;
        }
    }

    Check(TokenID::CLOSE_BRACE, "Missing close brace");
    depth--;
    Advance();
    std::shared_ptr<FuncDecl> func = std::make_shared<FuncDecl>(ret, name, argtypes, paramIdentifiers, body, beg);
    return func;
}

std::shared_ptr<Stmt> Parser::ParseStructDecl()
{
    if (depth >= 1)
        ParseError(cur, "Struct declarations are only allowed in the global region");

    Token loc = cur;
    Check(TokenID::STRUCT, "Struct declaration must begin with 'struct'");
    Advance();

    Check(TokenID::TYPENAME, "Struct declaration must be 'struct' followed by a type name");
    std::string name = cur.literal;
    Advance();

    TypeData parent = {0, 0};

    if (cur.type == TokenID::COLON)
    {
        Advance();
        parent = ParseType("Struct derived from another struct must have a valid type after the colon");
    }

    Check(TokenID::OPEN_BRACE, "Expect an open brace after struct declaration");
    Advance();

    std::vector<std::shared_ptr<Stmt>> decls;

    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        try
        {
            decls.push_back(VarDeclaration());
        }
        catch (const std::exception &e)
        {
            hadError = true;
            PanicMode({TokenID::SEMI, TokenID::END});
            Advance();
            std::cerr << e.what() << std::endl;
        }
    }

    Check(TokenID::CLOSE_BRACE, "Missing close brace");
    Advance();
    return std::make_shared<StructDecl>(name, parent, decls, loc);
}

// ----------------------STATEMENTS----------------------- //

std::shared_ptr<Block> Parser::ParseBlock()
{
    Advance();
    depth++;
    std::shared_ptr<Block> result = std::make_shared<Block>(depth, cur);
    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        try
        {
            if (cur.type == TokenID::OPEN_BRACE)
                result->stmts.push_back(ParseBlock());
            else
                result->stmts.push_back(Statement());
        }
        catch (const std::exception &e)
        {
            hadError = true;
            PanicMode({TokenID::SEMI, TokenID::END});
            Advance();
            std::cerr << e.what() << std::endl;
        }
    }

    if (cur.type == TokenID::CLOSE_BRACE)
        Advance();
    else
        ParseError(cur, "Need to close braces");

    depth--;
    return result;
}

std::shared_ptr<Stmt> Parser::IfStatement()
{
    Token loc = cur;
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
    return std::make_shared<IfStmt>(cond, thenBranch, elseBranch, loc);
}

std::shared_ptr<Stmt> Parser::WhileStatement()
{
    Token loc = cur;
    // skip the WHILE token
    Advance();

    Check(TokenID::OPEN_PAR, "Need an open parenthesis at the beginning of a while statement");
    //skipping over the OPEN_PAR
    Advance();

    std::shared_ptr<Expr> cond = Expression();
    Check(TokenID::CLOSE_PAR, "Missing close parenthesis");

    // skipping over the CLOSE_PAR
    Advance();

    std::shared_ptr<Stmt> body = Statement();

    return std::make_shared<WhileStmt>(cond, body, loc);
}

std::shared_ptr<Stmt> Parser::ExpressionStatement()
{
    Token loc = cur;
    std::shared_ptr<Expr> exp = Expression();

    Check(TokenID::SEMI, "Missing ';'");
    Advance();
    return std::make_shared<ExprStmt>(exp, loc);
}

std::shared_ptr<Stmt> Parser::ParseImportStmt()
{
    Token loc = cur;
    Advance();
    if (loc.type == TokenID::IMPORT)
    {
        std::vector<std::string> modules = CommaSeparatedStrings();
        std::vector<std::string> symbols;
        return std::make_shared<ImportStmt>(modules, symbols, loc);
    }
    else
    {
        std::vector<std::string> modules;
        modules.push_back(cur.literal);

        Advance();
        Check(TokenID::IMPORT, "Need an import section in a from/import statement");
        Advance();

        std::vector<std::string> symbols = CommaSeparatedStrings();
        return std::make_shared<ImportStmt>(modules, symbols, loc);
    }
}

std::vector<std::string> Parser::CommaSeparatedStrings()
{
    std::vector<std::string> strings{cur.literal};
    Advance();

    while (cur.type == TokenID::COMMA)
    {
        Advance();
        strings.push_back(cur.literal);
        Advance();
    }

    Check(TokenID::SEMI, "Need ';' at the end of an import statement");
    Advance();
    return strings;
}

// ----------------------EPRESSIONS----------------------- //

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
            std::shared_ptr<VarReference> u = std::make_shared<VarReference>(v->Loc());
            return std::make_shared<Assign>(u, val, loc);
        }
        else if (dynamic_cast<ArrayIndex *>(exp.get()) != nullptr)
            return std::make_shared<Assign>(exp, val, loc);
        else if (dynamic_cast<FieldAccess *>(exp.get()) != nullptr)
            return std::make_shared<Assign>(exp, val, loc);
        if (dynamic_cast<BracedInitialiser *>(val.get()) != nullptr)
            return std::make_shared<Assign>(exp, val, loc);
        else
            ParseError(cur, "Invalid assignment target/value");
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
        Token loc = cur;
        Advance();
        std::shared_ptr<Expr> right = std::make_shared<Unary>(loc, UnaryOp());
        return right;
    }

    return ParseArrayIndex(ParseFieldAccess());
}

std::shared_ptr<Expr> Parser::ParseArrayIndex(std::shared_ptr<Expr> name)
{
    if (cur.type == TokenID::OPEN_SQ)
    {
        Token loc = cur;
        Advance();

        std::shared_ptr<Expr> idx = Expression();
        Check(TokenID::CLOSE_SQ, "Missing ']'");
        Advance();

        std::shared_ptr<Expr> res = std::make_shared<ArrayIndex>(name, idx, loc);

        if (cur.type != TokenID::OPEN_SQ)
            return res;
        else
            return ParseArrayIndex(res);
    }
    return name;
}

std::shared_ptr<Expr> Parser::ParseFieldAccess()
{
    std::shared_ptr<Expr> left = LiteralNode();
    Token loc = cur;

    while (cur.type == TokenID::DOT)
    {
        Advance();
        std::shared_ptr<Expr> right = LiteralNode();
        left = std::make_shared<FieldAccess>(left, right, loc);
        loc = cur;
    }

    return left;
}

std::shared_ptr<Expr> Parser::LiteralNode()
{
    std::shared_ptr<Expr> res = nullptr;
    if (cur.type == TokenID::NULL_T || IsLiteral(cur))
        res = std::make_shared<Literal>(cur);
    else if (cur.type == TokenID::IDEN && (next.type == TokenID::OPEN_PAR || next.type == TokenID::LT))
        res = FuncCall();
    else if (cur.type == TokenID::OPEN_PAR)
    {
        Advance();
        res = Expression();
        if (cur.type != TokenID::CLOSE_PAR)
            ParseError(cur, "NEED TO CLOSE THE PARENS");
    }
    else if (cur.type == TokenID::IDEN)
    {
        Token loc = cur;
        res = std::make_shared<VarReference>(loc);
    }
    else if (cur.type == TokenID::TYPENAME || cur.type == TokenID::ARRAY || cur.type == TokenID::OPEN_BRACE)
        res = ParseBracedInitialiser();
    else
        ParseError(cur, "Misplaced token on line: " + std::to_string(cur.line) + "\nToken: '" + cur.literal + "'");

    Advance();
    return res;
}

std::shared_ptr<Expr> Parser::FuncCall()
{
    std::string name = cur.literal;
    Token loc = cur;
    // skipping the name
    Advance();

    TypeData type = {0, false};
    TypeData voidT = {0, false};

    if (cur.type == TokenID::LT)
    {
        Advance();
        type = ParseType("Invalid type for generic function call");
        Check(TokenID::GT, "Missing '>'");
        Advance();
    }

    std::vector<std::shared_ptr<Expr>> args;

    while (cur.type != TokenID::CLOSE_PAR && cur.type != TokenID::END)
    {
        Advance();
        if (cur.type != TokenID::COMMA && cur.type != TokenID::CLOSE_PAR)
            args.push_back(Expression());
    }

    Check(TokenID::CLOSE_PAR, "Need to close parenthesis");

    if (type == voidT)
        return std::make_shared<FunctionCall>(name, args, loc);
    else
    {
        if (name != "Cast")
            ParseError(loc, "Invalid generic function");

        if (args.size() != 1)
            ParseError(loc, "Cast takes 1 argument");

        return std::make_shared<TypeCast>(type, args[0], loc);
    }
}

std::shared_ptr<Expr> Parser::ParseBracedInitialiser()
{
    Token loc = cur;
    TypeData type = {0, false};

    if (cur.type != TokenID::OPEN_BRACE)
        type = ParseType("Malformed type in brace initialiser");

    if (cur.type == TokenID::OPEN_SQ)
    {
        Advance();
        std::shared_ptr<Expr> index = Expression();
        Check(TokenID::CLOSE_SQ, "Missing ']'");
        return std::make_shared<DynamicAllocArray>(type, index, loc);
    }

    Check(TokenID::OPEN_BRACE, "Braced initialiser starts with a type name and an open brace");
    // Advance();
    std::vector<std::shared_ptr<Expr>> init;

    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        Advance();
        if (cur.type != TokenID::COMMA && cur.type != TokenID::CLOSE_BRACE)
            init.push_back(Expression());
    }

    Check(TokenID::CLOSE_BRACE, "Missing close brace");
    std::shared_ptr<Expr> res = std::make_shared<BracedInitialiser>(init.size(), init, loc);
    res->t = type;
    return res;
}