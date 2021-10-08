#pragma once
#include "common.h"
#include "typedata.h"
#include <string>
#include <vector>

struct VarID
{
    TypeData type;
    std::string name;
    size_t depth;
    size_t size;
    bool isStructMember = false;
    VarID() = default;
    VarID(TypeData _type, std::string _name, size_t _depth, size_t _size) : type(_type), name(_name), depth(_depth), size(_size){};
};

enum class FunctionType
{
    USER_DEFINED,
    USER_DEFINED_TEMPLATE,
    LIBRARY,
    NATIVE
};

struct FuncID
{
    TypeData ret;
    std::string name;
    std::vector<TypeData> templates;
    std::vector<TypeData> argtypes;
    FunctionType kind;
    // necessary for trigerring analysis/compilation of
    // a template function upon encountering a template
    // function call
    size_t parseIndex;
    FuncID() = default;

    FuncID(const TypeData &_ret,
           const std::string &_name,
           const std::vector<TypeData> &_templates,
           const std::vector<TypeData> &_argtypes,
           const FunctionType &_kind,
           const size_t &_parseIndex)
        : ret(_ret),
          name(_name),
          templates(_templates),
          argtypes(_argtypes),
          kind(_kind),
          parseIndex(_parseIndex){};
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
    std::string name;
    TypeData type;
    TypeData parent;
    std::vector<std::pair<std::string, TypeData>> nameTypes;

    StructID(const std::string &_name,
             const TypeData &_type,
             const TypeData &_parent,
             const std::vector<std::pair<std::string, TypeData>> &_nameTypes)
        : name(_name),
          type(_type),
          parent(_parent),
          nameTypes(_nameTypes){};
};