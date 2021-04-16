#pragma once
#include "common.h"
#include "exprnode.h"
#include <string>
#include <vector>

struct VarID
{
    TypeData type;
    std::string name;
    uint8_t depth;
    uint8_t index;
    bool isStructMember = false;
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