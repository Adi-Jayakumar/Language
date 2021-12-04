#pragma once
#include "idstructs.h"
#include "nativefuncs.h"
#include "perror.h"
#include "templateutils.h"
#include <dlfcn.h>
#include <optional>
#include <sstream>

class TypeInfo
{
public:
    TypeData left;
    TokenID t;
    TypeData right;

    TypeInfo() = default;
    TypeInfo(TypeData _left, TokenID _t, TypeData _right) : left(_left), t(_t), right(_right){};
};

bool operator==(const TypeInfo &l, const TypeInfo &r);

struct TypeInfoHasher
{
    size_t operator()(const TypeInfo &t) const
    {
        std::hash<size_t> stHasher;
        std::hash<TypeID> tHasher;
        std::hash<uint8_t> u8Hasher;
        size_t l = stHasher(t.left.isArray) ^ tHasher(t.left.type);
        size_t r = stHasher(t.right.isArray) ^ tHasher(t.right.type);
        size_t op = u8Hasher(static_cast<uint8_t>(t.t));
        return l ^ r ^ op;
    }
};

class SymbolTable
{
public:
    std::vector<VarID> vars;

private:
    std::vector<FuncID> funcs;
    std::vector<FuncID> native_funcs;
    std::vector<FuncID> c_lib_functions;
    std::vector<StructID> strcts;

    std::unordered_map<TypeInfo, TypeData, TypeInfoHasher>
        operator_map{
            // binary plus
            {{INT_TYPE, TokenID::PLUS, INT_TYPE}, INT_TYPE},
            {{INT_TYPE, TokenID::PLUS, DOUBLE_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::PLUS, INT_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::PLUS, DOUBLE_TYPE}, DOUBLE_TYPE},
            // binary mins
            {{INT_TYPE, TokenID::MINUS, INT_TYPE}, INT_TYPE},
            {{INT_TYPE, TokenID::MINUS, DOUBLE_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::MINUS, INT_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::MINUS, DOUBLE_TYPE}, DOUBLE_TYPE},
            // unary minus
            {{VOID_TYPE, TokenID::MINUS, INT_TYPE}, INT_TYPE},
            {{VOID_TYPE, TokenID::MINUS, DOUBLE_TYPE}, DOUBLE_TYPE},
            // binary mult
            {{INT_TYPE, TokenID::STAR, INT_TYPE}, INT_TYPE},
            {{INT_TYPE, TokenID::STAR, DOUBLE_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::STAR, INT_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::STAR, DOUBLE_TYPE}, DOUBLE_TYPE},
            // binary div
            {{INT_TYPE, TokenID::SLASH, INT_TYPE}, INT_TYPE},
            {{INT_TYPE, TokenID::SLASH, DOUBLE_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::SLASH, INT_TYPE}, DOUBLE_TYPE},
            {{DOUBLE_TYPE, TokenID::SLASH, DOUBLE_TYPE}, DOUBLE_TYPE},
            // binary greater than
            {{INT_TYPE, TokenID::GT, INT_TYPE}, BOOL_TYPE},
            {{INT_TYPE, TokenID::GT, DOUBLE_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::GT, INT_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::GT, DOUBLE_TYPE}, BOOL_TYPE},
            // unary less than
            {{INT_TYPE, TokenID::LT, INT_TYPE}, BOOL_TYPE},
            {{INT_TYPE, TokenID::LT, DOUBLE_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::LT, INT_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::LT, DOUBLE_TYPE}, BOOL_TYPE},
            // binary greater than or equal
            {{INT_TYPE, TokenID::GEQ, INT_TYPE}, BOOL_TYPE},
            {{INT_TYPE, TokenID::GEQ, DOUBLE_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::GEQ, INT_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::GEQ, DOUBLE_TYPE}, BOOL_TYPE},
            // binary less than or equal
            {{INT_TYPE, TokenID::LEQ, INT_TYPE}, BOOL_TYPE},
            {{INT_TYPE, TokenID::LEQ, DOUBLE_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::LEQ, INT_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::LEQ, DOUBLE_TYPE}, BOOL_TYPE},
            // binary eqality
            {{INT_TYPE, TokenID::EQ_EQ, INT_TYPE}, BOOL_TYPE},
            {{INT_TYPE, TokenID::EQ_EQ, DOUBLE_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::EQ_EQ, INT_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::EQ_EQ, DOUBLE_TYPE}, BOOL_TYPE},
            {{BOOL_TYPE, TokenID::EQ_EQ, BOOL_TYPE}, BOOL_TYPE},
            // binary not equality
            {{INT_TYPE, TokenID::BANG_EQ, INT_TYPE}, BOOL_TYPE},
            {{INT_TYPE, TokenID::BANG_EQ, DOUBLE_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::BANG_EQ, INT_TYPE}, BOOL_TYPE},
            {{DOUBLE_TYPE, TokenID::BANG_EQ, DOUBLE_TYPE}, BOOL_TYPE},
            {{BOOL_TYPE, TokenID::BANG_EQ, BOOL_TYPE}, BOOL_TYPE},
            // binary booean and
            {{BOOL_TYPE, TokenID::AND_AND, BOOL_TYPE}, BOOL_TYPE},
            // binary boolean or
            {{BOOL_TYPE, TokenID::OR_OR, BOOL_TYPE}, BOOL_TYPE},
            // unary negation
            {{VOID_TYPE, TokenID::BANG, BOOL_TYPE}, BOOL_TYPE},
            // string concatenation
            {{STRING_TYPE, TokenID::PLUS, STRING_TYPE}, STRING_TYPE},
        };

    std::unordered_map<std::string, TypeData> type_name_map{{"void", VOID_TYPE},
                                                            {"int", INT_TYPE},
                                                            {"double", DOUBLE_TYPE},
                                                            {"bool", BOOL_TYPE},
                                                            {"string", STRING_TYPE},
                                                            {"char", CHAR_TYPE},
                                                            {"null_t", NULL_TYPE}};

    std::unordered_map<TypeID, std::string> type_string_map{{0, "void"},
                                                            {1, "int"},
                                                            {2, "double"},
                                                            {3, "bool"},
                                                            {4, "string"},
                                                            {5, "char"},
                                                            {6, "null_t"}};
    size_t num_types;

    size_t func_var_begin;
    size_t sp_offset;

public:
    size_t depth;
    SymbolTable() : num_types(NUM_DEF_TYPES), func_var_begin(0), sp_offset(0), depth(0)
    {
        for (const auto &func : NativeFunctions)
            native_funcs.push_back(ParseLibraryFunction(func, FunctionType::NATIVE));
    };

    bool CanAssign(const TypeData &target, const TypeData &given);

    // accepts any type apart from VOID_TYPE
    size_t SizeOf(const TypeData &type);

    //-------------------TYPE METHODS-------------------//

    TypeData AddType(const std::string &name);
    void RemoveType(const std::string &type);

    std::optional<TypeData> ResolveType(const std::string &type)
    {
        if (type_name_map.find(type) != type_name_map.end())
            return type_name_map[type];
        else
            return std::nullopt;
    };

    std::string ToString(const TypeData &type);
    void PrintType(std::ostream &out, const TypeData &type);

    //-------------------OPERATOR USE METHODS-------------------//

    bool CheckOperatorUse(const TypeData &left, const TokenID &op, const TypeData &right);
    TypeData OperatorResult(const TypeData &left, const TokenID &op, const TypeData &right);

    //-------------------BASE POINTER OFFSET METHODS-------------------//
    // returns the offset from the base pointer of the last variable added to the stack
    size_t GetCurOffset();
    void UpdateSP(const size_t &offset) { sp_offset += offset; };
    void ReduceSP(const size_t &offset) { sp_offset -= offset; };
    void SetSP(const size_t &newLoc) { sp_offset = newLoc; };
    size_t GetNewVarOffset();

    //-------------------ADD SYMBOLS-------------------//
    void AddVar(const TypeData &type, const std::string &name, const size_t size);
    void AddFunc(const FuncID &fid);
    void AddCLibFunc(const FuncID &fid);
    void AddStruct(const StructID &sid);

    //-------------------VARIABLE OPERATIONS-------------------//
    bool IsVarInScope(std::string &name);
    VarID *GetVar(std::string &name);              // getters of identifiers
    size_t GetVariableStackLoc(std::string &name); // returns SIZE_MAX if variable is not found

    //-------------------FUNCTION OPERATIONS-------------------//
    FuncID *GetFunc(std::string &name, std::vector<TypeData> &templates, std::vector<TypeData> &argtypes);
    size_t GetUDFuncNum(FuncID *fid);
    size_t GetCLibFuncNum(FuncID *fid);
    size_t NumCFuncs() { return c_lib_functions.size(); };
    size_t GetNativeFuncNum(FuncID *fid);

    bool MatchTemplateFunction(std::vector<TypeData> &templates, std::vector<TypeData> &args,
                               std::vector<TypeData> f_templates, std::vector<TypeData> f_args);
    FuncID *FindCLibraryFunctions(const std::vector<TypeData> &args, const std::string &name);

    //-------------------STRUCT OPERATIONS-------------------//
    StructID *GetStruct(const TypeData &type);

    //-------------------UTILITY WRAPPERS-------------------//
    bool IsEqual(const std::vector<TypeData> &actual, const std::vector<TypeData> &given);
    bool CanAssignAll(const std::vector<TypeData> &actual, const std::vector<TypeData> &given);

    //-------------------STACK OPERATIONS-------------------//
    void PopUntilSized(size_t size);
    void CleanUpCurDepth();

    //-------------------LIBRARY-------------------//
    TypeData ParseType(const std::string &type);
    std::vector<std::string> GetLibraryFunctionNames(const std::string &libname);
    FuncID ParseLibraryFunction(const std::string &func, const FunctionType type);
};

void SymbolError(const std::string &msg);
void LibraryError(const std::string &msg);

bool MatchToNativeFuncs(const std::vector<TypeData> &, const std::vector<TypeData> &);