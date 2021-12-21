#include "vm.h"

VM::VM(std::vector<Function> &_functions, oprand_t mainIndex,
       std::unordered_map<oprand_t, std::unordered_set<oprand_t>> &_StructTree,
       std::vector<LibraryFunctionDef> &_syms,
       std::vector<ThrowInfo> &_throwInfos)
{
    functions = _functions;
    struct_tree = _StructTree;
    throw_infos = _throwInfos;

    for (auto &lf : _syms)
    {
        std::string libpath = "./lib/lib" + lf.library + ".so";
        void *handle = dlopen(libpath.c_str(), RTLD_NOW);
        lib_handles.push_back(handle);

        LibFunc func;
        *(void **)&func = dlsym(handle, lf.name.c_str());
        CLibs.push_back({func, lf.arity});
    }

    cur_func = mainIndex == UINT8_MAX ? UINT8_MAX : 0;
    cs.push_back({0, 0, 0});
    cs.push_back({0, mainIndex, 0});
    cur_cf = &cs.back();

    ip = 0;
    cur_routine = 0;
}

void VM::Disasemble()
{
    for (size_t i = 0; i < functions.size(); i++)
    {
        std::cout << "Function index: " << i << std::endl
                  << "Function arity: " << +functions[i].arity
                  << std::endl
                  << std::endl;

        functions[i].PrintCode();

        std::cout << std::endl
                  << std::endl;
    }
}

void VM::PrintCallStack()
{
    for (auto &cf : cs)
        std::cout << "(" << cf.ret_index << ", " << cf.ret_function << ", " << cf.val_stack_min << ")" << std::endl;
}

void VM::RuntimeError(const std::string &msg)
{
    std::cerr << "[RUNTIME ERROR] " << msg << std::endl;
    exit(4);
}

void VM::Jump(size_t jump)
{
    ip += jump;
}

void VM::ExecuteProgram()
{
    if (cur_func == UINT8_MAX)
        return;
    while (true)
    {
        while (ip < functions[cur_func].routines[cur_routine].size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        if (!cs.empty())
        {

            CallFrame return_cf = *cur_cf;
            cs.pop_back();
            cur_cf = &cs.back();
            ip = return_cf.ret_index;

            if (cur_func != 0)
                ip++;

            cur_func = return_cf.ret_function;
            cur_routine = 0;
            ip = 0;
            size_t stack_diff = stack.GetSize() - return_cf.val_stack_min;

            // cleaning up the function's constants
            stack.ReduceSize(stack_diff);
        }
        else
            return;
    }
}

#define ERROR_OUT()                             \
    std::cerr << "Not implmented" << std::endl; \
    exit(3)

void VM::ExecuteInstruction()
{
    Op o = functions[cur_func].routines[cur_routine][ip];
    switch (o.code)
    {
    case Opcode::POP:
    {
        stack.PopBytes(o.op);
        break;
    }
    case Opcode::LOAD_INT:
    {
        stack.PushInt(functions[cur_func].ints[o.op]);
        break;
    }
    case Opcode::LOAD_DOUBLE:
    {
        stack.PushDouble(functions[cur_func].doubles[o.op]);
        break;
    }
    case Opcode::LOAD_BOOL:
    {
        stack.PushBool(functions[cur_func].bools[o.op]);
        break;
    }
    case Opcode::LOAD_STRING:
    {
        stack.PushString(functions[cur_func].strings[o.op]);
        break;
    }
    case Opcode::LOAD_CHAR:
    {
        stack.PushChar(functions[cur_func].chars[o.op]);
        break;
    }
    case Opcode::INT_ASSIGN:
    {
        stack.SetInt(o.op, stack.PeekInt());
        break;
    }
    case Opcode::DOUBLE_ASSIGN:
    {
        stack.SetDouble(o.op, stack.PeekDouble());
        break;
    }
    case Opcode::BOOL_ASSIGN:
    {
        stack.SetBool(o.op, stack.PeekBool());
        break;
    }
    case Opcode::STRING_ASSIGN:
    {
        int len = stack.PeekInt();
        char *str = stack.PeekPtr();
        stack.SetString(o.op, str, len);
        break;
    }
    case Opcode::CHAR_ASSIGN:
    {
        stack.SetChar(o.op, stack.PeekChar());
        break;
    }
    case Opcode::ARRAY_ASSIGN:
    {
        ERROR_OUT();
        break;
    }
    case Opcode::STRUCT_ASSIGN:
    {
        ERROR_OUT();
        break;
    }
    case Opcode::PUSH:
    {
        stack.PushOprandT(o.op);
        break;
    }
    case Opcode::PUSH_SP_OFFSET:
    {
        stack.PushPtr(stack.GetTop() + o.op);
        break;
    }
    case Opcode::GET_INT:
    {
        stack.PushInt(stack.GetInt(o.op));
        break;
    }
    case Opcode::GET_DOUBLE:
    {
        stack.PushDouble(stack.GetDouble(o.op));
        break;
    }
    case Opcode::GET_BOOL:
    {
        stack.PushBool(stack.GetBool(o.op));
        break;
    }
    case Opcode::GET_STRING:
    {
        char *str_len = stack.GetString(o.op);
        int len = *(int *)(str_len + PTR_SIZE);
        stack.PushString(str_len, len);
        break;
    }
    case Opcode::GET_CHAR:
    {
        stack.PushChar(stack.GetChar(o.op));
        break;
    }
    case Opcode::GET_ARRAY:
    {
        ERROR_OUT();
        break;
    }
    case Opcode::GET_STRUCT:
    {
        ERROR_OUT();
    }
    case Opcode::ARR_INDEX:
    {
        break;
    }
    case Opcode::ARR_SET:
    {
        break;
    }
    case Opcode::ARR_ALLOC:
    {
        break;
    }
    case Opcode::STRING_INDEX:
    {
        break;
    }
    case Opcode::STRING_SET:
    {
        break;
    }
    case Opcode::SET_IP:
    {
        ip = o.op;
        break;
    }
    case Opcode::GOTO_LABEL:
    {
        cur_routine = o.op;
        ip = -1;
        break;
    }
    case Opcode::GOTO_LABEL_IF_FALSE:
    {
        break;
    }
    case Opcode::CALL_F:
    {
        cs.push_back({ip, cur_func, stack.GetSize() - functions[o.op].arity});
        cur_cf = &cs.back();

        if (cs.size() > STACK_MAX)
            RuntimeError("CallStack overflow. Used: " + std::to_string(cs.size()) + " call-frames");

        cur_func = o.op;
        ip = -1;
        break;
    }
    case Opcode::CALL_LIBRARY_FUNC:
    {
        break;
    }
    case Opcode::RETURN:
    {
        CallFrame return_cf = *cur_cf;
        cs.pop_back();
        cur_cf = &cs.back();

        ip = return_cf.ret_index;
        cur_func = return_cf.ret_function;

        size_t stack_diff = stack.GetSize() - return_cf.val_stack_min;
        // Object *retVal = stack.back;

        // cleaning up the function's constants
        stack.PopBytes(stack_diff);
        // stack.push_back(retVal);
        break;
    }
    case Opcode::RETURN_VOID:
    {
        CallFrame return_cf = *cur_cf;
        cs.pop_back();
        cur_cf = &cs.back();

        ip = return_cf.ret_index;
        cur_func = return_cf.ret_function;

        size_t stack_diff = stack.GetSize() - return_cf.val_stack_min;

        // cleaning up the function's constants
        stack.PopBytes(stack_diff);
        break;
    }
    case Opcode::PUSH_THROW_INFO:
    {
        throw_stack.push(throw_infos[o.op]);
        break;
    }
    case Opcode::THROW:
    {
        break;
    }
    case Opcode::NATIVE_CALL:
    {
        oprand_t num_bytes = stack.PopOprandT();
        ReturnValue ret = natives[o.op](stack.GetTop() - num_bytes);
        if (ret != NULL_RETURN)
            stack.PushReturnValue(ret);
        break;
    }
    case Opcode::STRUCT_MEMBER:
    {
        break;
    }
    case Opcode::STRUCT_D:
    {
        break;
    }
    case Opcode::STRUCT_MEMBER_SET:
    {
        break;
    }
    case Opcode::CAST:
    {
        break;
    }
    // ADDITIONS: adds the last 2 things on the stack
    case Opcode::I_ADD:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushInt(l + r);
        std::cout << l + r << std::endl;
        break;
    }
    case Opcode::DI_ADD:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushDouble(l + r);
        break;
    }
    case Opcode::ID_ADD:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushDouble(l + r);
        break;
    }
    case Opcode::D_ADD:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushDouble(l + r);
        break;
    }
    case Opcode::S_ADD:
    {
        char *l = stack.PopString();
        char *r = stack.PopString();

        int l_len = *(int *)l;
        int r_len = *(int *)r;

        char *l_ptr = l + INT_SIZE;
        char *r_ptr = r + INT_SIZE;

        int new_len = l_len + r_len;
        char *new_ptr = new char[new_len];

        std::memcpy(new_ptr, l_ptr, l_len);
        char *next = new_ptr + l_len;
        std::memcpy(next, r_ptr, r_len);

        stack.PushInt(new_len);
        stack.PushPtr(new_ptr);
        break;
    }
    // SUBTRACTIONS: subtracts the last 2 things on the stack
    // if o.op is 1 then is a unary negation (only the case
    // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        int r = stack.PopInt();
        if (o.op != 0)
        {
            int l = stack.PopInt();
            stack.PushInt(l - r);
        }
        else
            stack.PushInt(-r);
        break;
    }
    case Opcode::DI_SUB:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushDouble(l - r);
        break;
    }
    case Opcode::ID_SUB:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushDouble(l - r);
        break;
    }
    case Opcode::D_SUB:
    {
        double r = stack.PopDouble();
        if (o.op != 0)
        {
            double l = stack.PopDouble();
            stack.PushDouble(l - r);
        }
        else
            stack.PushDouble(-r);
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushDouble(l * r);
        break;
    }
    case Opcode::DI_MUL:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushDouble(l * r);
        break;
    }
    case Opcode::ID_MUL:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushDouble(l * r);
        break;
    }
    case Opcode::D_MUL:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushDouble(l * r);
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushInt(l / r);
        break;
    }
    case Opcode::DI_DIV:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushDouble(l / r);
        break;
    }
    case Opcode::ID_DIV:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushDouble(l / r);
        break;
    }
    case Opcode::D_DIV:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushDouble(l / r);
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushBool(l > r);
        break;
    }
    case Opcode::DI_GT:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushBool(l > r);
        break;
    }
    case Opcode::ID_GT:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushBool(l > r);
        break;
    }
    case Opcode::D_GT:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushBool(l > r);
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushBool(l < r);
        break;
    }
    case Opcode::DI_LT:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushBool(l < r);
        break;
    }
    case Opcode::ID_LT:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushBool(l < r);
        break;
    }
    case Opcode::D_LT:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushBool(l < r);
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushBool(l >= r);
        break;
    }
    case Opcode::DI_GEQ:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushBool(l >= r);
        break;
    }
    case Opcode::ID_GEQ:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushBool(l >= r);
        break;
    }
    case Opcode::D_GEQ:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushBool(l >= r);
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushBool(l <= r);
        break;
    }
    case Opcode::DI_LEQ:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushBool(l <= r);
        break;
    }
    case Opcode::ID_LEQ:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushBool(l <= r);
        break;
    }
    case Opcode::D_LEQ:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushBool(l <= r);
        break;
    }
    case Opcode::N_EQ_EQ:
    {
        ERROR_OUT();
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushBool(l == r);
        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushBool(l == r);
        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        double r = stack.PopDouble();
        int l = stack.PopDouble();
        stack.PushBool(l == r);
        break;
    }
    case Opcode::D_EQ_EQ:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushBool(l == r);
        break;
    }
    case Opcode::B_EQ_EQ:
    {
        bool r = stack.PopBool();
        bool l = stack.PopBool();
        stack.PushBool(l == r);
        break;
    }
    case Opcode::N_BANG_EQ:
    {
        ERROR_OUT();
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        int r = stack.PopInt();
        int l = stack.PopInt();
        stack.PushBool(l != r);
        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        int r = stack.PopInt();
        double l = stack.PopDouble();
        stack.PushBool(l != r);
        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        double r = stack.PopDouble();
        int l = stack.PopInt();
        stack.PushBool(l != r);
        break;
    }
    case Opcode::D_BANG_EQ:
    {
        double r = stack.PopDouble();
        double l = stack.PopDouble();
        stack.PushBool(l != r);
        break;
    }
    case Opcode::B_BANG_EQ:
    {
        bool r = stack.PopBool();
        bool l = stack.PopBool();
        stack.PushBool(l != r);
        break;
    }
    case Opcode::B_AND_AND:
    {
        bool r = stack.PopBool();
        bool l = stack.PopBool();
        stack.PushBool(l && r);
        break;
    }
    case Opcode::B_OR_OR:
    {
        bool r = stack.PopBool();
        bool l = stack.PopBool();
        stack.PushBool(l || r);
        break;
    }
    case Opcode::BANG:
    {
        bool r = stack.PopBool();
        stack.PushBool(!r);
        break;
    }
    // Does nothing
    case Opcode::NONE:
    {
        break;
    }
    }
}

VM VM::DeserialiseProgram(const std::string &f_path)
{
    std::ifstream file;
    if (!DoesFileExist(f_path))
        DeserialisationError("File '" + f_path + "' does not exist");

    file.open(f_path, std::ios::in | std::ios::binary);

    oprand_t main_index;
    file.read((char *)&main_index, sizeof(main_index));

    oprand_t num_functions;
    file.read((char *)&num_functions, sizeof(num_functions));

    std::vector<Function> program;
    for (oprand_t i = 0; i < num_functions; i++)
        program.push_back(DeserialiseFunction(file));

    std::unordered_map<oprand_t, std::unordered_set<oprand_t>> struct_tree;
    std::vector<LibraryFunctionDef> lib_funcs;
    std::vector<ThrowInfo> throw_infos;

    while (file.peek() != EOF)
    {
        size_t id = ReadSizeT(file);
        switch (id)
        {
        case STRUCT_TREE_ID:
        {
            TypeID num_structs;
            file.read((char *)&num_structs, sizeof(num_structs));

            for (size_t i = 0; i < num_structs; i++)
            {
                TypeID struct_id;
                file.read((char *)&struct_id, sizeof(struct_id));

                size_t num_parents = ReadSizeT(file);

                for (size_t i = 0; i < num_parents; i++)
                {
                    TypeID parent;
                    file.read((char *)&parent, sizeof(parent));
                    struct_tree[struct_id].insert(parent);
                }
            }
            break;
        }
        case LIB_FUNC_ID:
        {
            size_t num_lib_funcs = ReadSizeT(file);

            for (size_t i = 0; i < num_lib_funcs; i++)
            {
                size_t name_len = ReadSizeT(file);
                ReadSizeT(file);
                char *c_name = (char *)DeserialiseData(name_len, sizeof(char), file);
                std::string name(c_name, name_len);
                delete[] c_name;

                size_t lib_len = ReadSizeT(file);
                ReadSizeT(file);
                char *c_lib_name = (char *)DeserialiseData(lib_len, sizeof(char), file);
                std::string lib_name(c_lib_name, lib_len);
                delete[] c_lib_name;

                size_t arity = ReadSizeT(file);
                lib_funcs.push_back(LibraryFunctionDef(name, lib_name, arity));
            }
            break;
        }
        case THROW_INFO_ID:
        {
            throw_infos = DeserialiseThrowInfos(file);
            break;
        }
        default:
        {
            DeserialisationError("Invalid section identifier " + std::to_string(id));
            break;
        }
        }
    }
    file.close();
    return VM(program, main_index, struct_tree, lib_funcs, throw_infos);
}

bool VM::DoesFileExist(const std::string &path)
{
    return std::ifstream(path).good();
}

void VM::DeserialisationError(const std::string &err)
{
    Error e = Error("[DE-SERIALISATION ERROR]\n" + err + "\n");
    throw e;
}

Function VM::DeserialiseFunction(std::ifstream &file)
{
    oprand_t arity;
    file.read((char *)&arity, sizeof(arity));

    std::vector<int> ints;
    std::vector<double> doubles;
    std::vector<bool> bools;
    std::vector<char> chars;
    std::vector<std::string> strings;
    std::vector<std::vector<Op>> code;

    for (size_t i = 0; i < 6; i++)
    {
        size_t type_code = ReadSizeT(file);
        switch (type_code)
        {
        case INT_ID:
        {
            ints = DeserialiseInts(file);
            break;
        }
        case DOUBLE_ID:
        {
            doubles = DeserialiseDoubles(file);
            break;
        }
        case BOOL_ID:
        {
            bools = DeserialiseBools(file);
            break;
        }
        case CHAR_ID:
        {
            chars = DeserialiseChars(file);
            break;
        }
        case STRING_ID:
        {
            strings = DeserialiseStrings(file);
            break;
        }
        case CODE_ID:
        {
            code = DeserialiseOps(file);
            break;
        }
        }
    }

    return Function(arity, code, ints, doubles, bools, chars, strings);
}

//=================================DE-SERIALISATION=================================//

size_t VM::ReadSizeT(std::ifstream &file)
{
    char c_num_elements[sizeof(size_t)];
    file.read(c_num_elements, sizeof(size_t));

    return *(size_t *)c_num_elements;
}

void *VM::DeserialiseData(size_t num_elements, size_t type_size, std::ifstream &file)
{
    char *c_data = new char[type_size * num_elements];

    file.read(c_data, type_size * num_elements);
    return c_data;
}

std::vector<int> VM::DeserialiseInts(std::ifstream &file)
{
    size_t num_ints = ReadSizeT(file);
    int *data = (int *)DeserialiseData(num_ints, sizeof(int), file);

    std::vector<int> result(data, data + num_ints);
    delete[] data;
    return result;
}

std::vector<double> VM::DeserialiseDoubles(std::ifstream &file)
{
    size_t num_doubles = ReadSizeT(file);
    double *data = (double *)DeserialiseData(num_doubles, sizeof(double), file);

    std::vector<double> result(data, data + num_doubles);
    delete[] data;
    return result;
}

std::vector<bool> VM::DeserialiseBools(std::ifstream &file)
{
    size_t num_bools = ReadSizeT(file);
    bool *data = (bool *)DeserialiseData(num_bools, sizeof(bool), file);

    std::vector<bool> result(data, data + num_bools);
    delete[] data;
    return result;
}

std::vector<char> VM::DeserialiseChars(std::ifstream &file)
{
    size_t num_chars = ReadSizeT(file);
    char *data = (char *)DeserialiseData(num_chars, sizeof(char), file);

    std::vector<char> result(data, data + num_chars);
    delete[] data;
    return result;
}

std::vector<std::string> VM::DeserialiseStrings(std::ifstream &file)
{
    size_t num_strings = ReadSizeT(file);
    std::vector<std::string> result;

    for (size_t i = 0; i < num_strings; i++)
    {
        size_t str_len = ReadSizeT(file);
        char *data = (char *)DeserialiseData(str_len, sizeof(char), file);
        result.push_back(std::string(data, str_len));
        delete[] data;
    }

    return result;
}

std::vector<std::vector<Op>> VM::DeserialiseOps(std::ifstream &file)
{
    size_t num_routines = ReadSizeT(file);
    std::vector<std::vector<Op>> result;

    for (size_t j = 0; j < num_routines; j++)
    {

        size_t num_ops = ReadSizeT(file);
        std::vector<Op> routine;

        for (size_t i = 0; i < num_ops; i++)
        {
            Opcode code;
            file.read((char *)&code, sizeof(code));

            oprand_t oprand;
            file.read((char *)&oprand, sizeof(oprand));
            routine.push_back(Op(code, oprand));
        }

        result.push_back(routine);
    }

    return result;
}

std::vector<ThrowInfo> VM::DeserialiseThrowInfos(std::ifstream &file)
{
    size_t num_throws = ReadSizeT(file);
    std::vector<ThrowInfo> result;

    for (size_t i = 0; i < num_throws; i++)
    {
        ThrowInfo ti = ThrowInfo();
        file.read((char *)&ti.is_array, sizeof(bool));
        file.read((char *)&ti.type, sizeof(ti.type));
        file.read((char *)&ti.func, sizeof(ti.func));
        file.read((char *)&ti.index, sizeof(ti.index));
        result.push_back(ti);
    }
    return result;
}