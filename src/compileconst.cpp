#include "compileconst.h"
#define DUMMYCC CompileConst(255, "") // to silence compiler warnings -- never returned due to typechecking

bool IsTruthy(const CompileConst &cc)
{
    switch (cc.type)
    {
    case 1:
    {
        return cc.as.i;
    }
    case 2:
    {
        return cc.as.d;
    }
    case 3:
    {
        return cc.as.b;
    }
    }
    return false;
}

CompileConst::CompileConst(TypeID _type, std::string literal)
{
    type = _type;
    switch (type)
    {
    case 1:
    {
        type = 1;
        as.i = std::stoi(literal);
        break;
    }
    case 2:
    {
        type = 2;
        as.d = std::stod(literal);
        break;
    }
    case 3:
    {
        type = 3;
        if (literal == "true")
            as.b = true;
        else
            as.b = false;
        break;
    }
    }
}

CompileConst::CompileConst(int _i)
{
    type = 1;
    as.i = _i;
}

CompileConst::CompileConst(double _d)
{
    type = 2;
    as.d = _d;
}

CompileConst::CompileConst(bool _b)
{
    type = 3;
    as.b = _b;
}

std::ostream &operator<<(std::ostream &out, const CompileConst &cc)
{
    switch (cc.type)
    {
    case UINT8_MAX:
    {
        out << cc.as.i;
        break;
    }
    case 1:
    {
        out << cc.as.i;
        break;
    }
    case 2:
    {
        out << cc.as.d;
        break;
    }
    case 3:
    {
        if (cc.as.b)
            out << "true";
        else
            out << "false";
        break;
    }
    }
    return out;
}

CompileVar::CompileVar(std::string &_name, CompileConst &_val)
{
    name = _name;
    val = _val;
}

std::ostream &operator<<(std::ostream &out, const CompileVar &cv)
{
    out << "Name: " << cv.name << " Value: " << cv.val;
    return out;
}

CompileConst operator+(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i + right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i + right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d + right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d + right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator-(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i - right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i - right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d - right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d - right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator*(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i * right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i * right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d * right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d * right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator/(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i / right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i / right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d / right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d / right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator>(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i > right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i > right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d > right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d > right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator<(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i < right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i < right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d < right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d < right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator>=(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i >= right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i >= right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d >= right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d >= right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator<=(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i <= right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i <= right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d <= right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d <= right.as.d);
        }
        }
        return DUMMYCC;
    }
    }
    return DUMMYCC;
}

CompileConst operator==(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i == right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i == right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d == right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d == right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 3:
    {
        return CompileConst(left.as.b == right.as.b);
    }
    }
    return DUMMYCC;
}

CompileConst operator!=(const CompileConst &left, const CompileConst &right)
{
    switch (left.type)
    {
    case 1:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.i != right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.i != right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 2:
    {
        switch (right.type)
        {
        case 1:
        {
            return CompileConst(left.as.d != right.as.i);
        }
        case 2:
        {
            return CompileConst(left.as.d != right.as.d);
        }
        }
        return DUMMYCC;
    }
    case 3:
    {
        return CompileConst(left.as.b != right.as.b);
    }
    }
    return DUMMYCC;
}