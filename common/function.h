#pragma once
#include "op.h"
#include <iostream>
#include <vector>

struct Function
{
    opcode_t arity;
    std::vector<Op> code;

    // constants
    std::vector<int> ints;
    std::vector<double> doubles;
    std::vector<bool> bools;
    std::vector<char> chars;
    std::vector<std::string> strings;

    Function() = default;
    Function(opcode_t _arity, std::vector<Op> _code, std::vector<int> _ints, std::vector<double> _doubles, std::vector<bool> _bools, std::vector<char> _chars, std::vector<std::string> _strings) : arity(_arity), code(_code), ints(_ints), doubles(_doubles), bools(_bools), chars(_chars), strings(_strings){};

    void PrintCode()
    {
        for (Op &o : code)
        {
            std::cout << ToString(o.code);
            if (o.code == Opcode::LOAD_INT)
                std::cout << " at index: " << +o.op << " value: " << ints[o.op];
            else if (o.code == Opcode::LOAD_DOUBLE)
                std::cout << " at index: " << +o.op << " value: " << doubles[o.op];
            else if (o.code == Opcode::LOAD_BOOL)
                std::cout << " at index: " << +o.op << " value: " << (bools[o.op] ? "true" : "false");
            else if (o.code == Opcode::LOAD_STRING)
                std::cout << " at index: " << +o.op << " value: " << strings[o.op];
            else if (o.code == Opcode::LOAD_CHAR)
                std::cout << " at index: " << +o.op << " value: " << chars[o.op];
            else if (o.code == Opcode::GET_V || o.code == Opcode::VAR_A)
                std::cout << " at relative stack index: " << +o.op;
            else
                std::cout << " " << +o.op;

            std::cout << std::endl;
        }
    }
};