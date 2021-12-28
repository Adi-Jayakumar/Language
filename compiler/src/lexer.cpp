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

size_t Lexer::GetColPos(size_t start)
{
    return start - cur_line_beg + 2;
}

Token Lexer::NextToken()
{
    SkipComment();
    SkipWhiteSpace();

    if (index == src.length())
        return Token(TokenID::END, "", line, GetColPos(index));

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
            size_t start = index;
            std::string name;
            while (isalnum(src[index]) || src[index] == '_')
            {
                name += src[index];
                index++;
            }
            // used for when custom types are added in the form of classes
            if (symbols->ResolveType(name))
                return Token(TokenID::TYPENAME, name, line, GetColPos(start));
            else
                return Token(TokenID::IDEN, name, line, GetColPos(start));
        }
    }

    Token res;

    switch (src[index])
    {
    case '+':
    {
        res = Token(TokenID::PLUS, "+", line, GetColPos(index));
        break;
    }
    case '-':
    {
        res = Token(TokenID::MINUS, "-", line, GetColPos(index));
        break;
    }
    case '*':
    {
        res = Token(TokenID::STAR, "*", line, GetColPos(index));
        break;
    }
    case '/':
    {
        res = Token(TokenID::SLASH, "/", line, GetColPos(index));
        break;
    }
    case '>':
    {
        if (src[index + 1] == '=')
        {
            res = Token(TokenID::GEQ, ">=", line, GetColPos(index));
            index++;
        }
        else
            res = Token(TokenID::GT, ">", line, GetColPos(index));
        break;
    }
    case '<':
    {
        if (src[index + 1] == '=')
        {
            res = Token(TokenID::LEQ, "<=", line, GetColPos(index));
            index++;
        }
        else
            res = Token(TokenID::LT, "<", line, GetColPos(index));
        break;
    }
    case '=':
    {
        if (src[index + 1] == '=')
        {
            res = Token(TokenID::EQ_EQ, "==", line, GetColPos(index));
            index++;
        }
        else
            res = Token(TokenID::EQ, "=", line, GetColPos(index));
        break;
    }
    case '!':
    {
        if (src[index + 1] == '=')
        {
            res = Token(TokenID::BANG_EQ, "!=", line, GetColPos(index));
            index++;
        }
        else
            res = Token(TokenID::BANG, "!", line, GetColPos(index));
        break;
    }
    case '&':
    {
        if (src[index + 1] == '&')
        {
            res = Token(TokenID::AND_AND, "&&", line, GetColPos(index));
            index++;
        }
        break;
    }
    case ';':
    {
        res = Token(TokenID::SEMI, ";", line, GetColPos(index));
        break;
    }
    case '(':
    {
        if (src[index + 1] == '|')
        {
            res = Token(TokenID::OPEN_VER, "(|", line, GetColPos(index));
            index++;
        }
        else
            res = Token(TokenID::OPEN_PAR, "(", line, GetColPos(index));
        break;
    }
    case ')':
    {
        res = Token(TokenID::CLOSE_PAR, ")", line, GetColPos(index));
        break;
    }
    case '|':
    {
        if (src[index + 1] == '|')
        {
            res = Token(TokenID::OR_OR, "||", line, GetColPos(index));
            index++;
        }
        else if (src[index + 1] == ')')
        {
            res = Token(TokenID::CLOSE_VER, "|)", line, GetColPos(index));
            index++;
        }
        break;
    }
    case '{':
    {
        res = Token(TokenID::OPEN_BRACE, "{", line, GetColPos(index));
        break;
    }
    case '}':
    {
        res = Token(TokenID::CLOSE_BRACE, "}", line, GetColPos(index));
        break;
    }
    case '[':
    {
        res = Token(TokenID::OPEN_SQ, "[", line, GetColPos(index));
        break;
    }
    case ']':
    {
        res = Token(TokenID::CLOSE_SQ, "]", line, GetColPos(index));
        break;
    }
    case ',':
    {
        res = Token(TokenID::COMMA, ",", line, GetColPos(index));
        break;
    }
    case '.':
    {
        res = Token(TokenID::DOT, ".", line, GetColPos(index));
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

        res = Token(TokenID::STRING_L, src.substr(start, end - start), line, GetColPos(start - 1));
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

        res = Token(TokenID::CHAR_L, literal, line, GetColPos(start - 1));
        break;
    }
    case ':':
    {
        res = Token(TokenID::COLON, ":", line, GetColPos(index));
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
        {
            line++;
            cur_line_beg = index + 1;
        }
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
            cur_line_beg = index;
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

    return Token(had_dot ? TokenID::DOUBLE_L : TokenID::INT_L, src.substr(start, length), line, GetColPos(start));
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
        tok = Token(t, src[index] + candidate, line, GetColPos(index));
        return true;
    }
    else
        return false;
}