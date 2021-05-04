#pragma once
#include "common.h"
#include "exprnode.h"
#include <string>
#include <vector>

struct VarID
{
    TypeData type;
    std::string name;
    size_t depth;
    size_t index;
    bool isStructMember = false;
    VarID() = default;
    VarID(TypeData _type, std::string _name, size_t _depth, size_t _index) : type(_type), name(_name), depth(_depth), index(_index){};
};

struct FuncID
{
    TypeData ret;
    std::string name;
    std::vector<TypeData> argtypes;
};

struct StructID
{
    TypeData type;
    TypeData parent;
    std::vector<std::string> memberNames;
    std::vector<TypeData> memTypes;
    std::vector<std::shared_ptr<Expr>> init;
    bool isNull;
    std::unordered_map<std::string, TypeData> nameTypes;
};