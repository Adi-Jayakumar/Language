#include "lexer.h"

std::string IO::GetSrcString(std::string file_name)
{
    std::ifstream in = std::ifstream(file_name);
    if (in.fail())
    {
        std::cerr << "File '" << file_name << "' does not exist" << std::endl;
        exit(2);
    }
    std::string src = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return src;
}

Lexer::Lexer(const std::string &f_path, SymbolTable *_symbols)
{
    index = 0;
    line = 1;
    src = IO::GetSrcString(f_path);
    symbols = _symbols;
}

void Lexer::LexError(std::string msg)
{
    Error e = Error("\n[LEX ERROR]: On line " + std::to_string(line) + "\n" + msg + "\n");
    throw e;
}

size_t Lexer::LineSize()
{
    size_t l = 0;
    size_t i = index;
    while (i != src.length() && src[i] != '\n')
    {
        l++;
        i++;
    }
    return l;
}

Token Lexer::NextToken()
{
    SkipComment();
    SkipWhiteSpace();

    if (index == src.length())
        return {TokenID::END, "", line};

    if (isdigit(src[index]))
        return LexNumber();

    if (isalpha(src[index]) || src[index] == '_')
    {
        Token t;
        if (CheckKeyword(t))
        {
            index += t.literal.length();
            return t;
        }
        else
        {
            std::string name;
            while (isalnum(src[index]) || src[index] == '_')
            {
                name += src[index];
                index++;
            }
            // used for when custom types are added in the form of classes
            if (symbols->ResolveType(name))
                return {TokenID::TYPENAME, name, line};
            else
                return {TokenID::IDEN, name, line};
        }
    }

    Token res;

    switch (src[index])
    {
    case '+':
    {
        res = {TokenID::PLUS, "+", line};
        break;
    }
    case '-':
    {
        res = {TokenID::MINUS, "-", line};
        break;
    }
    case '*':
    {
        res = {TokenID::STAR, "*", line};
        break;
    }
    case '/':
    {
        res = {TokenID::SLASH, "/", line};
        break;
    }
    case '>':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::GEQ, ">=", line};
            index++;
        }
        else
            res = {TokenID::GT, ">", line};
        break;
    }
    case '<':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::LEQ, "<=", line};
            index++;
        }
        else
            res = {TokenID::LT, "<", line};
        break;
    }
    case '=':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::EQ_EQ, "==", line};
            index++;
        }
        else
            res = {TokenID::EQ, "=", line};
        break;
    }
    case '!':
    {
        if (src[index + 1] == '=')
        {
            res = {TokenID::BANG_EQ, "!=", line};
            index++;
        }
        else
            res = {TokenID::BANG, "!", line};
        break;
    }
    case '&':
    {
        if (src[index + 1] == '&')
        {
            res = {TokenID::AND_AND, "&&", line};
            index++;
        }
        break;
    }
    case ';':
    {
        res = {TokenID::SEMI, ";", line};
        break;
    }
    case '(':
    {
        if (src[index + 1] == '|')
        {
            res = {TokenID::OPEN_VER, "(|", line};
            index++;
        }
        else
            res = {TokenID::OPEN_PAR, "(", line};
        break;
    }
    case ')':
    {
        res = {TokenID::CLOSE_PAR, ")", line};
        break;
    }
    case '|':
    {
        if (src[index + 1] == '|')
        {
            res = {TokenID::OR_OR, "||", line};
            index++;
        }
        else if (src[index + 1] == ')')
        {
            res = {TokenID::CLOSE_VER, "|)", line};
            index++;
        }
        break;
    }
    case '{':
    {
        res = {TokenID::OPEN_BRACE, "{", line};
        break;
    }
    case '}':
    {
        res = {TokenID::CLOSE_BRACE, "}", line};
        break;
    }
    case '[':
    {
        res = {TokenID::OPEN_SQ, "[", line};
        break;
    }
    case ']':
    {
        res = {TokenID::CLOSE_SQ, "]", line};
        break;
    }
    case ',':
    {
        res = {TokenID::COMMA, ",", line};
        break;
    }
    case '.':
    {
        res = {TokenID::DOT, ".", line};
        break;
    }
    case '"':
    {
        size_t start = index + 1;
        size_t end = start;

        while (src[end] != '"' && end != src.length() - 1)
            end++;

        if (end == src.length())
            LexError("Missing '\"'");

        index = end;

        res = {TokenID::STRING_L, src.substr(start, end - start), line};
        break;
    }
    case '\'':
    {
        size_t start = index + 1;
        size_t end = start;

        while (src[end] != '\'' && end != src.length() - 1)
            end++;

        if (end == src.length())
            LexError("Missing '\''");

        index = end;

        std::string literal = src.substr(start, end - start);

        if (literal.size() > 1)
        {
            // to be made more advanced in time
            LexError("Invalid character literal");
        }

        res = {TokenID::CHAR_L, literal, line};
        break;
    }
    case ':':
    {
        res = {TokenID::COLON, ":", line};
        break;
    }
    default:
    {
        size_t line_size = LineSize();
        LexError("Unkown token on line " + std::to_string(line) + "\nNear:\n '" + src.substr(index, index + std::min(5U, (unsigned)line_size)) + "'");
        break;
    }
    }

    index++;
    return res;
}

void Lexer::SkipWhiteSpace()
{
    if (index == src.length() - 1)
    {
        if (isspace(src[index]))
            index++;
        return;
    }

    while (isspace(src[index]))
    {
        if (src[index] == '\n')
            line++;
        index++;
    }
}

void Lexer::SkipComment()
{
    SkipWhiteSpace();

    if (index > src.length() - 2 && index < src.length())
    {
        if (src[index] != '/')
            return;
    }

    while (src[index] == '/' && src[index + 1] == '/')
    {
        index += 2;
        while (src[index] != '\n' && index < src.length())
            index++;

        if (src[index] == '\n')
        {
            index++;
            line++;
            SkipWhiteSpace();
        }
    }
}

Token Lexer::LexNumber()
{
    int start = index;
    int length = 0;

    bool had_dot = false;
    int e_count = 0;

    while (isdigit(src[index]) || src[index] == '.' || src[index] == 'e')
    {
        if (src[index] == '.')
            had_dot = true;
        if ((src[index] == 'e' && !had_dot) || e_count > 1)
            LexError("Malformed double on line " + std::to_string(line));

        if (src[index] == 'e')
        {
            if (src[index + 1] != '+' && src[index + 1] != '-')
                LexError("Malformed double on line " + std::to_string(line));
            index++;
            length++;
        }

        index++;
        length++;
    }

    return {had_dot ? TokenID::DOUBLE_L : TokenID::INT_L, src.substr(start, length), line};
}

bool Lexer::CheckKeyword(Token &tok)
{
    switch (src[index])
    {
    case 'A':
    {
        return MatchKeyWord("rray", TokenID::ARRAY, tok);
    }
    case 'b':
    {
        return MatchKeyWord("ool", TokenID::TYPENAME, tok) ||
               MatchKeyWord("reak", TokenID::BREAK, tok);
    }
    case 'c':
    {
        return MatchKeyWord("atch", TokenID::CATCH, tok) ||
               MatchKeyWord("har", TokenID::TYPENAME, tok);
    }
    case 'C':
    {
        return MatchKeyWord("ast", TokenID::CAST, tok);
    }
    case 'd':
    {
        return MatchKeyWord("ouble", TokenID::TYPENAME, tok);
    }
    case 'e':
    {
        return MatchKeyWord("lse", TokenID::ELSE, tok);
    }
    case 'f':
    {
        return MatchKeyWord("unction", TokenID::FUNC, tok) ||
               MatchKeyWord("alse", TokenID::BOOL_L, tok) ||
               MatchKeyWord("rom", TokenID::FROM, tok);
    }
    case 'i':
    {
        return MatchKeyWord("nt", TokenID::TYPENAME, tok) ||
               MatchKeyWord("f", TokenID::IF, tok) ||
               MatchKeyWord("mport", TokenID::IMPORT, tok);
    }
    case 'n':
    {
        return MatchKeyWord("ull", TokenID::NULL_T, tok);
    }
    case 'r':
    {
        return MatchKeyWord("eturn", TokenID::RETURN, tok) ||
               MatchKeyWord("esult", TokenID::RESULT, tok);
    }
    case 's':
    {
        return MatchKeyWord("tring", TokenID::TYPENAME, tok) ||
               MatchKeyWord("truct", TokenID::STRUCT, tok);
    }
    case 'S':
    {
        return MatchKeyWord("equence", TokenID::SEQUENCE, tok);
    }
    case 't':
    {
        return MatchKeyWord("emplate", TokenID::TEMPLATE, tok) ||
               MatchKeyWord("hrow", TokenID::THROW, tok) ||
               MatchKeyWord("ry", TokenID::TRY, tok) ||
               MatchKeyWord("rue", TokenID::BOOL_L, tok) ||
               MatchKeyWord("ypename", TokenID::TYPENAME_KW, tok);
    }
    case 'v':
    {
        return MatchKeyWord("oid", TokenID::TYPENAME, tok);
    }
    case 'w':
    {
        return MatchKeyWord("hile", TokenID::WHILE, tok);
    }
    }
    return false;
}

bool Lexer::MatchKeyWord(std::string kw, TokenID t, Token &tok)
{
    std::string candidate = src.substr(index + 1, kw.length());
    if (candidate == kw)
    {
        tok = {t, src[index] + candidate, line};
        return true;
    }
    else
        return false;
}