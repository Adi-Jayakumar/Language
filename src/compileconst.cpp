#include "compileconst.h"

CompileConst::CompileConst(TypeID _type, std::string &literal)
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
    }
    }
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
}