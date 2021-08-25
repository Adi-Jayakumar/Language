#pragma once
#include <string>
#include <vector>
#include "typedata.h"
#include "common.h"

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

    FuncID(TypeData _ret,
           std::string _name,
           std::vector<TypeData> _templates,
           std::vector<TypeData> _argtypes,
           FunctionType _kind,
           size_t _parseIndex)
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
    std::vector<std::string> memberNames;
    std::vector<TypeData> memTypes;
    std::unordered_map<std::string, TypeData> nameTypes;

    StructID(std::string _name,
             TypeData _type,
             TypeData _parent,
             std::vector<std::string> _memberNames,
             std::vector<TypeData> _memTypes,
             std::unordered_map<std::string, TypeData> _nameTypes)
        : name(_name),
          type(_type),
          parent(_parent),
          memberNames(_memberNames),
          memTypes(_memTypes),
          nameTypes(_nameTypes){};
};