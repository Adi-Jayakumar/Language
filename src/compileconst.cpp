#include "compileconst.h"
#define DUMMYCC CompileConst(255, "") // to silence compiler warnings -- never returned due to typechecking

std::string ToString(const CompileConst &cc)
{
    switch (cc.t.type)
    {
    case 1:
    {
        return std::to_string(cc.as.i);
    }
    case 2:
    {
        return std::to_string(cc.as.d);
    }
    case 3:
    {
        if (cc.as.b)
            return "true";
        else
            return "false";
    }
    }
    return "";
}

bool IsTruthy(const CompileConst &cc)
{
    switch (cc.t.type)
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

CompileConst::CompileConst(TypeData _type, std::string literal)
{
    t = _type;
    switch (t.type)
    {
    // sentinel null value
    case 0:
    {
        as.i = 0;
        break;
    }
    case 1:
    {
        as.i = std::stoi(literal);
        break;
    }
    case 2:
    {
        as.d = std::stod(literal);
        break;
    }
    case 3:
    {
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
    t = {false, 1};
    as.i = _i;
}

CompileConst::CompileConst(double _d)
{
    t = {false, 2};
    as.d = _d;
}

CompileConst::CompileConst(bool _b)
{
    t = {false, 3};
    as.b = _b;
}

CompileConst::CompileConst(size_t _size)
{
    t = {true, 4};
    as.arr.data = (CompileConst *)malloc(_size * sizeof(CompileConst));
    as.arr.size = _size;
}

CompileConst::CompileConst(CCArray _arr)
{
    t = {true, 4};
    as.arr = _arr;
}

std::ostream &operator<<(std::ostream &out, const CompileConst &cc)
{
    switch (cc.t.type)
    {
    case 0:
    {
        out << "null";
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
    case 4:
    {
        CCArray arr = cc.as.arr;
        out << "{";

        for(size_t i = 0; i < arr.size; i++)
        {
            out << arr.data[i];
            
            if(i != arr.size - 1)
                out << ", ";
        }

        out << "}";
    }
    }
    return out;
}