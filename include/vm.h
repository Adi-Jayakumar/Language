#pragma once
#include "compiler.h"

struct CompileVar
{
    std::string name;
    uint8_t depth;
    CompileConst decl;
    CompileVar(std::string, uint8_t);
};

// CompileVar::CompileVar(std::string _name, uint8_t _depth)
// {
//     name = _name;
//     std::string null = "-1";
//     decl = {UINT8_MAX, null};
//     depth = _depth;
// }

struct VM
{
    std::vector<Op> code;
    std::vector<CompileConst> constants;
    std::vector<CompileVar> vars;
};