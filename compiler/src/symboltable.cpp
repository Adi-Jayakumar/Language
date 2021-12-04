#include "symboltable.h"

bool operator==(const TypeInfo &l, const TypeInfo &r)
{
    return (l.t == r.t) && (l.left == r.left) && (l.right == r.right);
}

TypeData SymbolTable::AddType(const std::string &name)
{
    TypeData new_type(0, num_types++);
    type_name_map[name] = new_type;
    type_string_map[new_type.type] = name;
    return new_type;
}

void SymbolTable::RemoveType(const std::string &type)
{
    TypeID type_id = type_name_map[type].type;
    type_name_map.erase(type);
    type_string_map.erase(type_id);
}

std::string SymbolTable::ToString(const TypeData &type)
{
    if (type.is_array == 0)
        return type_string_map[type.type];
    else
    {
        std::string res("Array<");
        res += std::to_string(type.is_array);
        res += ", " + type_string_map[type.type] + ">";
        return res;
    }
}

void SymbolTable::PrintType(std::ostream &out, const TypeData &type)
{
    if (type.is_array == 0)
        out << type_string_map[type.type];
    else
        out << "Array<" << type.is_array << ", " << type_string_map[type.type] << ">";
}

bool SymbolTable::CheckOperatorUse(const TypeData &left, const TokenID &op, const TypeData &right)
{
    return operator_map.find(TypeInfo(left, op, right)) != operator_map.end();
}

TypeData SymbolTable::OperatorResult(const TypeData &left, const TokenID &op, const TypeData &right)
{
    return operator_map.at(TypeInfo(left, op, right));
}

bool SymbolTable::CanAssign(const TypeData &varType, const TypeData &valType)
{
    if (varType.is_array != valType.is_array)
        return false;

    if (varType.type == 6 || valType.type == 6)
        return true;
    else if (varType.type == 1 && valType.type == 2)
        return true;
    else if (varType.type == 2 && valType.type == 1)
        return true;

    if (varType.type > 6 && valType.type > 6)
    {
        if (varType == valType)
            return true;

        StructID *sVal = GetStruct(valType);
        TypeData parent = sVal->parent;

        if (parent == VOID_TYPE)
            return varType == valType;

        parent.is_array = valType.is_array;
        return CanAssign(varType, parent);
    }

    return varType == valType;
}

size_t SymbolTable::SizeOf(const TypeData &type)
{
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

bool SymbolTable::IsVarInScope(std::string &name)
{
    size_t varIndex = SIZE_MAX;

    for (size_t i = vars.size() - 1; (int)i >= 0; i--)
    {
        if (vars[i].name == name && vars[i].depth == depth)
        {
            varIndex = i;
            break;
        }
    }

    return varIndex != SIZE_MAX;
}

VarID *SymbolTable::GetVar(std::string &name)
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
        return nullptr;

    return &vars[varIndex];
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
    funcs.emplace_back(func);
}

void SymbolTable::AddCLibFunc(const FuncID &func)
{
    c_lib_functions.push_back(func);
}

FuncID *SymbolTable::GetFunc(std::string &name, std::vector<TypeData> &templates, std::vector<TypeData> &args)
{
    for (auto &f : funcs)
    {
        if (f.name == name && IsEqual(f.argtypes, args))
            return &f;
        if (f.name == name && MatchTemplateFunction(templates, args, f.templates, f.argtypes))
            return &f;
    }

    for (auto &f : funcs)
    {
        if (f.name == name && CanAssignAll(f.argtypes, args))
            return &f;
    }

    FuncID *f = FindCLibraryFunctions(args, name);
    if (f != nullptr)
        return f;

    for (auto &f : native_funcs)
    {
        if (f.name == name && IsEqual(f.argtypes, args))
            return &f;
    }

    return nullptr;
}

size_t SymbolTable::GetUDFuncNum(FuncID *fid)
{
    for (size_t i = 0; i < funcs.size(); i++)
    {
        if (fid == &funcs[i])
            return i;
    }
    return SIZE_MAX;
}

size_t SymbolTable::GetCLibFuncNum(FuncID *fid)
{
    for (size_t i = 0; i < c_lib_functions.size(); i++)
    {
        if (fid == &c_lib_functions[i])
            return i;
    }
    return SIZE_MAX;
}

size_t SymbolTable::GetNativeFuncNum(FuncID *fid)
{
    for (size_t i = 0; i < native_funcs.size(); i++)
    {
        if (fid == &native_funcs[i])
            return i;
    }
    return SIZE_MAX;
}

bool SymbolTable::MatchTemplateFunction(std::vector<TypeData> &templates, std::vector<TypeData> &args,
                                        std::vector<TypeData> f_templates, std::vector<TypeData> f_args)
{
    if (templates.size() != f_templates.size() || args.size() != f_args.size())
        return false;

    std::unordered_map<TypeID, TypeData> templateMap = GetTemplateMap();
    for (size_t i = 0; i < templates.size(); i++)
        templateMap[f_templates[i].type] = templates[i];

    for (size_t j = 0; j < f_args.size(); j++)
    {
        TypeData replacement = templateMap[f_args[j].type];
        f_args[j].type = replacement.type;
        f_args[j].is_array += replacement.is_array;
    }

    return IsEqual(f_args, args) || CanAssignAll(f_args, args);
}

FuncID *SymbolTable::FindCLibraryFunctions(const std::vector<TypeData> &args, const std::string &name)
{
    for (auto &lf : c_lib_functions)
    {
        if (lf.name == name && IsEqual(lf.argtypes, args))
            return &lf;
    }

    for (auto &lf : c_lib_functions)
    {
        if (lf.name == name && CanAssignAll(lf.argtypes, args))
            return &lf;
    }
    return nullptr;
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

StructID *SymbolTable::GetStruct(const TypeData &td)
{
    for (size_t i = strcts.size() - 1; (int)i >= 0; i--)
    {
        if (strcts[i].type.type == td.type)
            return &strcts[i];
    }

    return nullptr;
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
    if (type_name_map.find(type) != type_name_map.end())
        return type_name_map[type];

    static const std::string array("Array");

    if (!std::equal(array.begin(), array.end(), type.begin()))
        LibraryError("Invalid type '" + type + "'");

    if (type[5] != '<' || type[type.length() - 1] != '>')
        LibraryError("Invalid type '" + type + "'");

    std::string num_type(type.begin() + 6, type.end() - 1);
    std::cout << "numtype = " << num_type << std::endl;
    std::stringstream ss(num_type);

    std::string sNum;

    if (!std::getline(ss, sNum, ','))
    {
        if (type_name_map.find(num_type) != type_name_map.end())
            return type_name_map[num_type];
        else
            LibraryError("Invalid type '" + type + "'");
    }

    size_t num = std::stol(sNum);
    std::string sType;

    if (!std::getline(ss, sType, ','))
        LibraryError("Invalid type '" + type + "'");

    if (type_name_map.find(num_type) != type_name_map.end())
    {
        TypeData res = type_name_map[num_type];
        res.is_array = num;
        return res;
    }
    else
        LibraryError("Invalid type '" + type + "'");
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