#include "symboltable.h"

void SymbolTable::__print_type_string_map()
{
    for (auto &kv : type_string_map)
    {
        if (kv.first >= NUM_DEF_TYPES)
            std::cout << +kv.first << " --> " << kv.second << std::endl;
    }
}

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

TypeID SymbolTable::GenerateNewTypeID()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<TypeID> distro;

    TypeID candidate_id = distro(gen);

    if (active_types.size() == MAX_TYPE)
        assert(false != true); // TODO handle more gracefully

    while (active_types.count(candidate_id))
        candidate_id = distro(gen);

    return candidate_id;

    // std::cout << "I SHOULD NOT BE PRINTED" << std::endl;
    // assert(false); // TODO handle more gracefully
    // return 0;      // should never reach here
}

TypeData SymbolTable::AddType(const std::string &name)
{
    TypeData new_type(0, GenerateNewTypeID());
    type_string_map[new_type.type] = name;
    return new_type;
}

void SymbolTable::RemoveType(const TypeID type_id)
{
    assert(BuiltInTypeIDs.count(type_id) == 0);
    type_string_map.erase(type_id);
    --num_types;
}

std::optional<TypeData> SymbolTable::ResolveType(const std::string &type)
{
    for (const auto &kv : type_string_map)
    {
        if (kv.second == type)
            return TypeData(0, kv.first);
    }
    return std::nullopt;
}

std::optional<StructID> SymbolTable::InitialiseTemplateStructIfRequired(const TypeData &type)
{
    std::optional<StructID> is_already_def = GetStruct(type);
    if (is_already_def)
        return std::nullopt;

    if (type.tmps.size() == 0)
        return std::nullopt;

    TypeData base_type(type.is_array, type.type);
    std::optional<StructID> base = GetStruct(base_type);

    if (!base)
        return std::nullopt;

    if (base->tmps.size() != type.tmps.size())
        return std::nullopt;

    TypeSubstituter strct_subst(base->tmps, type.tmps, Token());

    std::vector<std::pair<std::string, TypeData>> name_types;
    for (size_t i = 0; i < base->name_types.size(); i++)
        name_types.push_back({base->name_types[i].first, strct_subst[base->name_types[i].second]});

    std::string s_type = ToString(type);
    StructID new_type(s_type, type, VOID_TYPE, name_types);

    strcts.push_back(new_type);
    return new_type;
}

std::string SymbolTable::ToString(const TypeData &type)
{
    if (type.is_array == 0)
    {
        std::string base_type = type_string_map[type.type];

        if (type.tmps.size() > 0)
        {
            base_type += "<";

            for (size_t i = 0; i < type.tmps.size() - 1; i++)
                base_type += ToString(type.tmps[i]) + ", ";

            base_type += ToString(type.tmps.back()) + ">";
        }

        return base_type;
    }
    else
    {
        std::string res("Array<");
        TypeData elem_type(type.is_array - 1, type.type, type.tmps);
        res += ToString(elem_type) + ">";
        return res;
    }
}

void SymbolTable::PrintType(std::ostream &out, const TypeData &type)
{
    out << ToString(type);
}

std::optional<TypeData> SymbolTable::OperatorResult(const TypeData &left, const TokenID &op, const TypeData &right)
{
    if (operator_map.find(TypeInfo(left, op, right)) != operator_map.end())
        return operator_map.at(TypeInfo(left, op, right));
    else
        return std::nullopt;
}

bool SymbolTable::CanAssign(const TypeData &var_type, const TypeData &val_type)
{
    if (var_type == val_type)
        return true;
    else if (var_type.tmps.size() != val_type.tmps.size()) // can never assign 'thing<X>' to 'thing<Y>' or 'other'
        return false;
    else if (var_type == INT_TYPE)
        return INT_ASSIGNABLES.count(val_type);
    else if (var_type == DOUBLE_TYPE)
        return DOUBLE_ASSIGNABLES.count(val_type);
    else if (var_type == BOOL_TYPE)
        return BOOL_ASSIGNABLES.count(val_type);
    else if (var_type == STRING_TYPE)
        return STRING_ASSIGNABLES.count(val_type);
    else if (var_type == CHAR_TYPE)
        return CHAR_ASSIGNABLES.count(val_type);
    else if (!BuiltInTypeIDs.count(var_type.type) && !BuiltInTypeIDs.count(val_type.type))
    {
        std::optional<StructID> sid_val_type = GetStruct(val_type);

        if (!sid_val_type)
            return false;

        return CanAssign(var_type, sid_val_type->parent);
    }
    else
        return false;
}

size_t SymbolTable::SizeOf(const TypeData &type)
{
    assert(type != VOID_TYPE);
    if (type.is_array)
        return ARRAY_SIZE;
    else if (type == INT_TYPE)
        return INT_SIZE;
    else if (type == DOUBLE_TYPE)
        return DOUBLE_SIZE;
    else if (type == BOOL_TYPE)
        return BOOL_SIZE;
    else if (type == STRING_TYPE)
        return STRING_SIZE;
    else if (type == CHAR_TYPE)
        return CHAR_SIZE;
    else if (type == NULL_TYPE)
        return NULL_SIZE;
    else
        return STRUCT_SIZE;
}

size_t SymbolTable::GetCurOffset()
{
    return sp_offset;
}

size_t SymbolTable::GetNewVarOffset()
{
    return sp_offset + (vars.size() ? SizeOf(vars.back().type) : 0);
}

void SymbolTable::AddVar(const TypeData &type, const std::string &name, const size_t size)
{
    vars.push_back(VarID(type, name, depth, size));
}

std::optional<VarID> SymbolTable::GetVar(std::string &name)
{
    size_t varIndex = SIZE_MAX;

    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name)
        {
            varIndex = i;
            break;
        }
    }

    if (varIndex == SIZE_MAX)
        return std::nullopt;

    return vars[varIndex];
}

size_t SymbolTable::GetVariableStackLoc(std::string &name)
{
    size_t loc = 0;
    for (const auto &var : vars)
    {
        if (name == var.name)
            return loc;
        loc += var.size;
    }

    return SIZE_MAX;
}

void SymbolTable::AddFunc(const FuncID &func)
{
    plain_funcs.emplace_back(func);
}

void SymbolTable::AddTemplateFunc(const FuncID &func)
{
    template_funcs.push_back(func);
}

void SymbolTable::AddInitialisedTemplateFunc(const FuncID &func)
{
    initialised_template_funcs.push_back(func);
}

void SymbolTable::AddCLibFunc(const FuncID &func)
{
    c_lib_funcs.push_back(func);
}

std::optional<FuncID> SymbolTable::GetFunc(const std::string &name,
                                           std::vector<TypeData> &args)
{
    for (auto &f : plain_funcs)
    {
        if (f.name == name && IsEqual(f.argtypes, args))
            return f;
    }

    for (auto &f : plain_funcs)
    {
        if (f.name == name && CanAssignAll(f.argtypes, args))
            return f;
    }

    for (auto &f : template_funcs)
    {
        if (MatchUninitialisedTemplateFunc(name, args, f))
            return f;
    }

    std::optional<FuncID> f = FindCLibraryFunctions(args, name);
    if (f)
        return f;

    for (auto &f : native_funcs)
    {
        if (f.name == name && IsEqual(f.argtypes, args))
            return f;
    }

    return std::nullopt;
}

size_t SymbolTable::GetUDFuncNum(std::optional<FuncID> &fid)
{
    for (size_t i = 0; i < plain_funcs.size(); i++)
    {
        if (FuncIDEq()(fid.value(), plain_funcs[i]))
            return i;
    }
    return SIZE_MAX;
}

size_t SymbolTable::GetCLibFuncNum(std::optional<FuncID> &fid)
{
    for (size_t i = 0; i < c_lib_funcs.size(); i++)
    {
        if (FuncIDEq()(fid.value(), c_lib_funcs[i]))
            return i;
    }
    return SIZE_MAX;
}

size_t SymbolTable::GetNativeFuncNum(std::optional<FuncID> &fid)
{
    for (size_t i = 0; i < native_funcs.size(); i++)
    {
        if (FuncIDEq()(fid.value(), native_funcs[i]))
            return i;
    }
    return SIZE_MAX;
}

std::optional<FuncID> SymbolTable::FindCLibraryFunctions(const std::vector<TypeData> &args, const std::string &name)
{
    for (auto &lf : c_lib_funcs)
    {
        if (lf.name == name && IsEqual(lf.argtypes, args))
            return lf;
    }

    for (auto &lf : c_lib_funcs)
    {
        if (lf.name == name && CanAssignAll(lf.argtypes, args))
            return lf;
    }

    return std::nullopt;
}

std::optional<FuncID> SymbolTable::MatchUninitialisedTemplateFunc(const std::string &name,
                                                                  std::vector<TypeData> &args,
                                                                  FuncID &fid)
{
    if (name != fid.name || args.size() != fid.argtypes.size())
        return std::nullopt;

    for (size_t i = 0; i < args.size(); i++)
    {
        if (args[i].is_array != fid.argtypes[i].is_array)
            return std::nullopt;
    }

    return fid;
}

std::optional<FuncID> SymbolTable::IsInitialisedTemplateFunc(const std::string &name,
                                                             const std::vector<TypeData> &templates,
                                                             const std::vector<TypeData> &args)
{
    for (auto &init : initialised_template_funcs)
    {
        if (init.name == name &&
            IsEqual(init.templates, templates) &&
            IsEqual(init.argtypes, args))
            return init;
    }

    return std::nullopt;
}

bool SymbolTable::IsEqual(const std::vector<TypeData> &actual, const std::vector<TypeData> &given)
{
    if (actual.size() != given.size())
        return false;

    bool equal = true;
    for (size_t i = 0; i < actual.size(); i++)
    {
        if (actual[i] != given[i])
        {
            equal = false;
            break;
        }
    }
    return equal;
}

bool SymbolTable::CanAssignAll(const std::vector<TypeData> &actual, const std::vector<TypeData> &given)
{
    if (actual.size() != given.size())
        return false;

    bool equal = true;
    for (size_t i = 0; i < actual.size(); i++)
    {
        if (!CanAssign(actual[i], given[i]))
        {
            equal = false;
            break;
        }
    }
    return equal;
}

void SymbolTable::PopUntilSized(size_t size)
{
    if (vars.size() <= size)
        return;

    while (vars.size() > size)
        vars.pop_back();
}

void SymbolTable::CleanUpCurDepth()
{
    while (vars.size() > 0 && vars.back().depth == depth)
        vars.pop_back();

    if (vars.size() == 1 && vars[0].depth == depth)
        vars.clear();
}

void SymbolTable::AddStruct(const StructID &s)
{
    strcts.push_back(s);
}

std::optional<StructID> SymbolTable::GetStruct(const TypeData &td)
{
    for (size_t i = strcts.size() - 1; (int)i >= 0; i--)
    {
        if (strcts[i].type == td)
            return strcts[i];
    }

    return std::nullopt;
}

void SymbolError(const std::string &msg)
{
    Error e("[Symbol ERROR]\n" + msg + "\n");
    throw e;
}

void LibraryError(const std::string &msg)
{
    Error e("[LIBRARY ERROR]\n" + msg + "\n");
    throw e;
}

std::vector<std::string> SplitStringByChar(std::string &s, char c)
{
    std::stringstream stream(s);
    std::string segment;
    std::vector<std::string> split;

    while (std::getline(stream, segment, c))
        split.push_back(segment);

    return split;
}

std::string TrimFrontBack(std::string &str)
{
    size_t first = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        first = i;
        if (!isspace(str[i]))
            break;
    }

    size_t last = str.length() - 1;
    for (size_t j = str.length() - 1; (int)j >= 0; j--)
    {
        last = j;
        if (!isspace(str[j]))
            break;
    }

    return str.substr(first, last - first + 1);
}

#define DL_ERROR(errorMsg)                  \
    errorMsg = dlerror();                   \
    if (errorMsg != NULL)                   \
    {                                       \
        std::cerr << errorMsg << std::endl; \
        exit(3);                            \
    }

std::vector<std::string> SymbolTable::GetLibraryFunctionNames(const std::string &modulename)
{
    std::string libpath = "../runtime/lib/lib" + modulename + ".so";
    char *errorMsg;

    void *handle = dlopen(libpath.c_str(), RTLD_LAZY);
    DL_ERROR(errorMsg)

    const char *const *LibraryFunctions;
    *(void **)&LibraryFunctions = dlsym(handle, "LibraryFunctions");
    DL_ERROR(errorMsg)

    size_t *numfuncs;
    *(void **)&numfuncs = dlsym(handle, "NumLibFunctions");
    DL_ERROR(errorMsg)

    std::vector<std::string> lib_funcs;
    for (size_t i = 0; i < *numfuncs; i++)
        lib_funcs.push_back(LibraryFunctions[i]);

    dlclose(handle);
    return lib_funcs;
}

TypeData SymbolTable::ParseType(const std::string &type)
{
    // std::optional<TypeData> td_type = ResolveType(type);
    // if (td_type)
    //     return td_type.value();

    // static const std::string array("Array");

    // if (!std::equal(array.begin(), array.end(), type.begin()))
    //     LibraryError("Invalid type '" + type + "'");

    // if (type[5] != '<' || type[type.length() - 1] != '>')
    //     LibraryError("Invalid type '" + type + "'");

    // std::string num_type(type.begin() + 6, type.end() - 1);
    // std::stringstream ss(num_type);

    // std::string sNum;

    // if (!std::getline(ss, sNum, ','))
    // {
    //     std::optional<TypeData> arr_type = ResolveType(sNum);
    //     if (type_name_map.find(num_type) != type_name_map.end())
    //         return type_name_map[num_type];
    //     else
    //         LibraryError("Invalid type '" + type + "'");
    // }

    // size_t num = std::stol(sNum);
    // std::string sType;

    // if (!std::getline(ss, sType, ','))
    //     LibraryError("Invalid type '" + type + "'");

    // if (type_name_map.find(num_type) != type_name_map.end())
    // {
    //     TypeData res = type_name_map[num_type];
    //     res.is_array = num;
    //     return res;
    // }
    // else
    //     LibraryError("Invalid type '" + type + "'");
    return VOID_TYPE;
}

FuncID SymbolTable::ParseLibraryFunction(const std::string &func, const FunctionType type)
{
    std::stringstream ss(func);
    std::string ret;

    if (!std::getline(ss, ret, ' '))
        LibraryError("Invalid function name '" + func + "'");

    TypeData retType = ParseType(ret);
    std::string name;

    if (!std::getline(ss, name, '('))
        LibraryError("Invalid function name '" + func + "'");

    std::string args;
    std::vector<TypeData> argtypes;

    if (!std::getline(ss, args, ')'))
        LibraryError("Invalid function name '" + func + "'");

    std::string arg;
    std::stringstream argstream(args);

    while (std::getline(argstream, arg, ' '))
        argtypes.push_back(ParseType(arg));

    return FuncID(retType, name, std::vector<TypeData>(), argtypes, type, 0);
}