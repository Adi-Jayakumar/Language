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
    FuncID() = default;
    FuncID(TypeData _ret, std::string _name, std::vector<TypeData> _argtypes) : ret(_ret), name(_name), argtypes(_argtypes){};
};

struct FuncIDEq
{
    bool operator()(const FuncID &left, const FuncID &right) const
    {
        if (left.argtypes.size() != right.argtypes.size())
            return false;

        bool ans = (left.ret == right.ret) && (left.name == right.name);

        for (size_t i = 0; i < left.argtypes.size(); i++)
            ans = (ans || left.argtypes[i] == right.argtypes[i]);

        return ans;
    }
};

struct FuncIDHasher
{
    size_t operator()(const FuncID &fi) const
    {
        TypeDataHasher t;
        size_t argHash = 0;

        for (const auto &arg : fi.argtypes)
            argHash = argHash ^ t(arg);

        std::hash<std::string> strHasher;
        size_t nameHash = strHasher(fi.name);

        size_t retHash = t(fi.ret);
        return argHash ^ nameHash ^ retHash;
    }
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