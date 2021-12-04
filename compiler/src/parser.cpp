#include "parser.h"
#include "ASTPrinter.h"

Parser::Parser(const std::string &fPath, SymbolTable &_symbols)
    : symbols(_symbols), lex(fPath, &symbols), depth(0), hadError(false)
{
    cur = lex.NextToken();
    next = lex.NextToken();
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

    if (prev.type == TokenID::STRUCT && cur.type == TokenID::TYPENAME_KW)
        symbols.AddType(next.literal);
}

TypeData Parser::ParseType(std::string err)
{
    if (cur.type == TokenID::TYPENAME)
    {
        std::string sType = cur.literal;
        Advance();
        return symbols.ResolveType(sType).value();
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
            type = symbols.ResolveType(cur.literal).value();
            Advance();
        }
        else
        {
            Check(TokenID::TYPENAME, "1 dimensional array requires type name");
            type = symbols.ResolveType(cur.literal).value();
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

std::vector<SP<Stmt>> Parser::Parse()
{
    std::vector<SP<Stmt>> res;
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

std::vector<SP<Expr>> Parser::ParseVerCondition()
{
    std::vector<SP<Expr>> post;
    if (cur.type == TokenID::OPEN_VER)
    {
        Advance();
        while (cur.type != TokenID::CLOSE_VER && cur.type != TokenID::END)
        {
            post.push_back(Expression());
            Check(TokenID::SEMI, "Expect semicolon after verification expressions");
            Advance();
        }

        Check(TokenID::CLOSE_VER, "Expect close verification");
        Advance();
    }
    return post;
}

SP<Stmt> Parser::Statement()
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
        SP<Expr> retVal = Expression();

        // checking for the semicolon
        Check(TokenID::SEMI, "Require ';' at the end of a return statement");

        // advancing over the semicolon
        Advance();
        return std::make_shared<Return>(retVal, cur);
    }
    else if (cur.type == TokenID::IMPORT || cur.type == TokenID::FROM)
        return ParseImportStmt();
    else if (cur.type == TokenID::BREAK)
    {
        Token loc = cur;
        Advance();
        Check(TokenID::SEMI, "Expect ';' after 'break'");
        Advance();
        return std::make_shared<Break>(loc);
    }
    else if (cur.type == TokenID::THROW)
    {
        Token loc = cur;
        Advance();
        SP<Expr> exp = Expression();
        Check(TokenID::SEMI, "Expect ';' after expression of throw");
        Advance();
        return std::make_shared<Throw>(exp, loc);
    }
    else if (cur.type == TokenID::TRY)
    {
        Token loc = cur;
        Advance();

        SP<Stmt> tryClause = Statement();

        Check(TokenID::CATCH, "Expect 'catch' after 'try' block");

        Advance();
        Check(TokenID::OPEN_PAR, "Expect '(' after 'catch'");

        Advance();
        TypeData catchType = ParseType("Expect type in catch statement");

        Check(TokenID::IDEN, "Expect identifier after type in catch statement");
        std::string catchVarName = cur.literal;
        std::pair<TypeData, std::string> catchVar(catchType, catchVarName);

        Advance();
        Check(TokenID::CLOSE_PAR, "Expect ')'");

        Advance();
        SP<Stmt> catchClause = Statement();

        return std::make_shared<TryCatch>(tryClause, catchClause, catchVar, loc);
    }
    return Declaration();
}

// ----------------------DECLARATIONS----------------------- //

SP<Stmt> Parser::Declaration()
{
    if (cur.type == TokenID::TYPENAME || cur.type == TokenID::ARRAY)
        return VarDeclaration();
    else if (cur.type == TokenID::FUNC)
        return FuncDeclaration();
    else if (cur.type == TokenID::STRUCT)
        return ParseStructDecl();
    else if (cur.type == TokenID::TEMPLATE)
        return TemplateFunction();
    else
        return ExpressionStatement();
}

SP<Stmt> Parser::VarDeclaration()
{
    TypeData type = ParseType("Expect type name at the beginning of a variable decaration");
    if (type == VOID_TYPE)
        ParseError(cur, "A variable cannot have 'void' type");

    Check(TokenID::IDEN, "Expect identifier after type name of a variable declaration");

    std::string name = cur.literal;
    Token loc = cur;

    Advance();
    SP<Expr> init = nullptr;

    if (cur.type == TokenID::EQ)
    {
        Advance();
        init = Expression();
    }

    Check(TokenID::SEMI, "Expect ';' after variable declaration");
    Advance();

    return std::make_shared<DeclaredVar>(type, name, init, loc);
}

SP<Stmt> Parser::FuncDeclaration()
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

    std::vector<std::pair<TypeData, std::string>> params;

    while (cur.type != TokenID::CLOSE_PAR && cur.type != TokenID::END)
    {
        try
        {
            TypeData argtype = ParseType("Function arguments need types");
            Check(TokenID::IDEN, "Function arguments need names");
            std::string param = cur.literal;
            params.push_back({argtype, param});

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

    std::vector<SP<Expr>> pre = ParseVerCondition();
    Check(TokenID::OPEN_BRACE, "Function body must start with an open brace");
    depth++;

    Advance();

    std::vector<SP<Stmt>> body;

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

    SP<Expr> post = nullptr;
    if (cur.type == TokenID::OPEN_VER)
    {
        Advance();
        post = Expression();
        Check(TokenID::CLOSE_VER, "Expect '|)' after post condition");
        Advance();
    }

    SP<FuncDecl> func = std::make_shared<FuncDecl>(ret, name, params, body, pre, post, beg);
    return func;
}

SP<Stmt> Parser::ParseStructDecl()
{
    if (depth >= 1)
        ParseError(cur, "Struct declarations are only allowed in the global region");

    Token loc = cur;
    Check(TokenID::STRUCT, "Struct declaration must begin with 'struct'");
    Advance();

    Check(TokenID::TYPENAME_KW, "Must have keyword 'typename' here");
    Advance();

    Check(TokenID::IDEN, "Struct declaration must be 'struct' followed by a type name");
    std::string name = cur.literal;
    Advance();

    TypeData parent = VOID_TYPE;

    if (cur.type == TokenID::COLON)
    {
        Advance();
        parent = ParseType("Struct derived from another struct must have a valid type after the colon");
    }

    Check(TokenID::OPEN_BRACE, "Expect an open brace after struct declaration");
    Advance();

    std::vector<SP<Stmt>> decls;

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

SP<Stmt> Parser::TemplateFunction()
{
    Token loc = cur;
    Advance();
    Check(TokenID::OPEN_TEMPLATE, "Expect '<|' after 'template'");

    std::vector<std::pair<TypeData, std::string>> addedTypes;

    size_t count = 0;
    while (cur.type != TokenID::CLOSE_TEMPLATE && cur.type != TokenID::END)
    {
        Advance();
        Check(TokenID::IDEN, "Expect type identifier");

        TypeData new_type = symbols.AddType(cur.literal);

        addedTypes.push_back({new_type, cur.literal});
        count++;
        Advance();
    }

    Check(TokenID::CLOSE_TEMPLATE, "Expect '|>' after template declaration");
    Advance();

    SP<Stmt> function = Statement();

    for (const auto &type_name : addedTypes)
        symbols.RemoveType(type_name.second);

    FuncDecl *fd = dynamic_cast<FuncDecl *>(function.get());
    if (fd == nullptr)
        ParseError(loc, "Can only have a templated function");
    fd->templates = addedTypes;
    return function;
}

// ----------------------STATEMENTS----------------------- //

SP<Block> Parser::ParseBlock()
{
    Advance();
    depth++;
    SP<Block> result = std::make_shared<Block>(cur);
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

SP<Stmt> Parser::IfStatement()
{
    Token loc = cur;
    Advance();
    Check(TokenID::OPEN_PAR, "Need an open paranthesis at the beginning of an if statement");
    Advance();
    SP<Expr> cond = Expression();
    Check(TokenID::CLOSE_PAR, "Missing a close parenthesis");
    Advance();
    SP<Stmt> thenBranch = Statement();
    SP<Stmt> elseBranch = nullptr;
    if (cur.type == TokenID::ELSE)
    {
        Advance();
        elseBranch = Statement();
    }
    return std::make_shared<IfStmt>(cond, thenBranch, elseBranch, loc);
}

SP<Stmt> Parser::WhileStatement()
{
    Token loc = cur;
    // skip the WHILE token
    Advance();

    Check(TokenID::OPEN_PAR, "Need an open parenthesis at the beginning of a while statement");
    //skipping over the OPEN_PAR
    Advance();

    SP<Expr> cond = Expression();
    Check(TokenID::CLOSE_PAR, "Missing close parenthesis");

    // skipping over the CLOSE_PAR
    Advance();

    SP<Stmt> body = Statement();

    return std::make_shared<WhileStmt>(cond, body, loc);
}

SP<Stmt> Parser::ExpressionStatement()
{
    Token loc = cur;
    SP<Expr> exp = Expression();

    Check(TokenID::SEMI, "Missing ';'");
    Advance();
    return std::make_shared<ExprStmt>(exp, loc);
}

SP<Stmt> Parser::ParseImportStmt()
{
    Token loc = cur;
    Check(TokenID::IMPORT, "Expect 'import' before import statements");
    Advance();
    std::vector<std::string> libraries = CommaSeparatedStrings();
    return std::make_shared<ImportStmt>(libraries, loc);
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

SP<Expr> Parser::Expression()
{
    return Or();
}

SP<Expr> Parser::ParseSequenceNode()
{
    Token loc = cur;
    Check(TokenID::SEQUENCE, "Expect 'Sequence' at the beginning of a sequence declaration");
    Advance();
    Check(TokenID::OPEN_PAR, "Expect '(' after Sequence declaration");
    Advance();

    SP<Expr> start = Expression();
    Check(TokenID::COMMA, "Expect ',' after start of sequence");
    Advance();

    SP<Expr> step = Expression();
    Check(TokenID::COMMA, "Expect ',' after sequence step");
    Advance();

    SP<Expr> end = Expression();
    Check(TokenID::COMMA, "Expect ',' after sequence end");
    Advance();

    Check(TokenID::IDEN, "Expect index variable");
    SP<VarReference> var = std::make_shared<VarReference>(cur);
    Advance();
    Check(TokenID::COMMA, "Expect ',' after sequence index variable");
    Advance();

    SP<Expr> term = Expression();
    Check(TokenID::COMMA, "Expect ',' after sequence general term");
    Advance();

    CheckBinaryOperator(cur.type);
    TokenID op = cur.type;
    Advance();
    Check(TokenID::CLOSE_PAR, "Expect ')' at end of sequence declaration");
    return std::make_shared<Sequence>(start, step, end, var, term, op, loc);
}

SP<Expr> Parser::Or()
{
    SP<Expr> left = And();
    Token op = cur;

    while (cur.type == TokenID::OR_OR)
    {
        Advance();
        SP<Expr> right = And();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

SP<Expr> Parser::And()
{
    SP<Expr> left = Assignment();
    Token op = cur;

    while (cur.type == TokenID::AND_AND)
    {
        Advance();
        SP<Expr> right = Assignment();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

SP<Expr> Parser::Assignment()
{
    SP<Expr> exp = EqualityCheck();

    if (cur.type == TokenID::EQ)
    {
        Token loc = cur;
        Advance();
        SP<Expr> val = Assignment();

        VarReference *v = dynamic_cast<VarReference *>(exp.get());
        if (v != nullptr)
        {
            SP<VarReference> u = std::make_shared<VarReference>(v->Loc());
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

SP<Expr> Parser::EqualityCheck()
{
    SP<Expr> left = Comparison();
    Token op = cur;

    while (cur.type == TokenID::EQ_EQ || cur.type == TokenID::BANG_EQ)
    {
        Advance();
        SP<Expr> right = Comparison();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

SP<Expr> Parser::Comparison()
{
    SP<Expr> left = Sum();
    Token op = cur;

    while (cur.type == TokenID::GT || cur.type == TokenID::LT || cur.type == TokenID::GEQ || cur.type == TokenID::LEQ)
    {
        Advance();
        SP<Expr> right = Sum();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

SP<Expr> Parser::Sum()
{
    SP<Expr> left = Product();
    Token op = cur;

    while (cur.type == TokenID::PLUS || cur.type == TokenID::MINUS)
    {
        Advance();
        SP<Expr> right = Product();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

SP<Expr> Parser::Product()
{
    SP<Expr> left = UnaryOp();
    Token op = cur;

    while (cur.type == TokenID::STAR || cur.type == TokenID::SLASH)
    {
        Advance();
        SP<Expr> right = UnaryOp();
        left = std::make_shared<Binary>(left, op, right);
        op = cur;
    }

    return left;
}

SP<Expr> Parser::UnaryOp()
{
    if (cur.type == TokenID::MINUS || cur.type == TokenID::BANG)
    {
        Token loc = cur;
        Advance();
        SP<Expr> right = std::make_shared<Unary>(loc, UnaryOp());
        return right;
    }

    return ParseArrayIndex(ParseFieldAccess());
}

SP<Expr> Parser::ParseArrayIndex(SP<Expr> name)
{
    if (cur.type == TokenID::OPEN_SQ)
    {
        Token loc = cur;
        Advance();

        SP<Expr> idx = Expression();
        Check(TokenID::CLOSE_SQ, "Missing ']'");
        Advance();

        SP<Expr> res = std::make_shared<ArrayIndex>(name, idx, loc);

        if (cur.type != TokenID::OPEN_SQ)
            return res;
        else
            return ParseArrayIndex(res);
    }
    return name;
}

SP<Expr> Parser::ParseFieldAccess()
{
    SP<Expr> left = LiteralNode();
    Token loc = cur;

    while (cur.type == TokenID::DOT)
    {
        Advance();
        SP<Expr> right = LiteralNode();
        left = std::make_shared<FieldAccess>(left, right, loc);
        loc = cur;
    }

    return left;
}

SP<Expr> Parser::LiteralNode()
{
    SP<Expr> res = nullptr;
    if (cur.type == TokenID::NULL_T || IsLiteral(cur))
        res = std::make_shared<Literal>(cur);
    else if (cur.type == TokenID::SEQUENCE && next.type == TokenID::OPEN_PAR)
        res = ParseSequenceNode();
    else if (cur.type == TokenID::IDEN && next.type == TokenID::OPEN_PAR)
        res = FuncCall();
    else if (cur.type == TokenID::IDEN && next.type == TokenID::OPEN_TEMPLATE)
        res = FuncCall();
    else if (cur.type == TokenID::CAST)
        res = FuncCall();
    else if (cur.type == TokenID::OPEN_PAR)
    {
        Advance();
        res = Expression();
        if (cur.type != TokenID::CLOSE_PAR)
            ParseError(cur, "NEED TO CLOSE THE PARENS");
    }
    else if (cur.type == TokenID::IDEN)
        res = std::make_shared<VarReference>(cur);
    else if (cur.type == TokenID::RESULT)
        res = std::make_shared<VarReference>(cur);
    else if (cur.type == TokenID::TYPENAME || cur.type == TokenID::ARRAY || cur.type == TokenID::OPEN_BRACE)
        res = ParseBracedInitialiser();
    else
        ParseError(cur, "Misplaced token on line: " + std::to_string(cur.line) + "\nToken: '" + cur.literal + "'");

    Advance();
    return res;
}

SP<Expr> Parser::FuncCall()
{
    std::string name = cur.literal;
    Token loc = cur;
    // skipping the name
    Advance();

    TypeData type = VOID_TYPE;
    std::vector<TypeData> templates;

    if (cur.type == TokenID::LT)
    {
        Advance();
        type = ParseType("Invalid type for generic function call");
        Check(TokenID::GT, "Missing '>'");
        Advance();
    }
    else if (cur.type == TokenID::OPEN_TEMPLATE)
    {
        while (cur.type != TokenID::CLOSE_TEMPLATE && cur.type != TokenID::END)
        {
            Advance();
            templates.push_back(ParseType("Expect type in a template list"));
        }

        Check(TokenID::CLOSE_TEMPLATE, "Missing '|>'");
        Advance();
    }

    std::vector<SP<Expr>> args;

    while (cur.type != TokenID::CLOSE_PAR && cur.type != TokenID::END)
    {
        Advance();
        if (cur.type != TokenID::COMMA && cur.type != TokenID::CLOSE_PAR)
            args.push_back(Expression());
    }

    Check(TokenID::CLOSE_PAR, "Need to close parenthesis");
    if (name == "Cast")
        return std::make_shared<TypeCast>(type, args[0], loc);
    else
        return std::make_shared<FunctionCall>(name, templates, args, loc);
}

SP<Expr> Parser::ParseBracedInitialiser()
{
    Token loc = cur;
    TypeData type = VOID_TYPE;

    if (cur.type != TokenID::OPEN_BRACE)
        type = ParseType("Malformed type in brace initialiser");

    if (cur.type == TokenID::OPEN_SQ)
    {
        Advance();
        SP<Expr> index = Expression();
        Check(TokenID::CLOSE_SQ, "Missing ']'");
        return std::make_shared<DynamicAllocArray>(type, index, loc);
    }

    Check(TokenID::OPEN_BRACE, "Braced initialiser starts with a type name and an open brace");
    // Advance();
    std::vector<SP<Expr>> init;

    while (cur.type != TokenID::CLOSE_BRACE && cur.type != TokenID::END)
    {
        Advance();
        if (cur.type != TokenID::COMMA && cur.type != TokenID::CLOSE_BRACE)
            init.push_back(Expression());
    }

    Check(TokenID::CLOSE_BRACE, "Missing close brace");
    SP<BracedInitialiser> res = std::make_shared<BracedInitialiser>(init.size(), init, loc);
    res->t = type;
    return res;
}