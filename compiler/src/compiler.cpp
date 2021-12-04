#include "compiler.h"

Compiler::Compiler()
{
    functions.push_back(Function());
    functions[0].arity = 0;
    cur = &functions[0];

    cur_routine = {&cur->routines[0], 0};
}

void Compiler::CompileError(Token loc, std::string err)
{
    Error e = Error("[COMPILE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void Compiler::AddCode(Op o)
{
    cur_routine.first->push_back(o);
}

size_t Compiler::CodeSize()
{
    return cur_routine.first->size();
}

std::pair<size_t, size_t> Compiler::LastAddedCodeLoc()
{
    return {GetCurRoutineIndex(), cur_routine.first->size() - 1};
}

void Compiler::ModifyOprandAt(std::pair<size_t, size_t> loc, oprand_t oprand)
{
    cur->routines[loc.first][loc.second].op = oprand;
}

void Compiler::ModifyOpcodeAt(std::pair<size_t, size_t> loc, Opcode opcode)
{
    cur->routines[loc.first][loc.second].code = opcode;
}

void Compiler::AddRoutine()
{
    cur->routines.push_back(std::vector<Op>());
    cur_routine = {&cur->routines.back(), cur->routines.size() - 1};
}

size_t Compiler::GetCurRoutineIndex()
{
    return cur_routine.second;
}

void Compiler::AddFunction()
{
    functions.push_back(Function());
    cur = &functions.back();
    cur_routine = {&cur->routines.back(), cur->routines.size() - 1};
}

size_t Compiler::GetVariableStackLoc(std::string &name)
{
    return symbols.GetVariableStackLoc(name);
}

void Compiler::Compile(std::vector<SP<Stmt>> &s)
{
    main_index = MAX_OPRAND;
    size_t num_functions = 0;
    for (parse_index = 0; parse_index < s.size(); parse_index++)
    {
        s[parse_index]->NodeCompile(*this);
        if (dynamic_cast<FuncDecl *>(s[parse_index].get()) != nullptr)
        {
            num_functions++;
            FuncDecl *asFD = static_cast<FuncDecl *>(s[parse_index].get());
            if (asFD->params.size() == 0 && asFD->name == "Main")
            {
                if (main_index != MAX_OPRAND)
                    CompileError(asFD->Loc(), "Main function already defined");
                main_index = num_functions;
            }
        }
        else if (dynamic_cast<DeclaredVar *>(s[parse_index].get()) == nullptr &&
                 dynamic_cast<StructDecl *>(s[parse_index].get()) == nullptr &&
                 dynamic_cast<ImportStmt *>(s[parse_index].get()) == nullptr)
            CompileError(s[parse_index]->Loc(), "Only declarations allowed in global region");
    }
}

void Compiler::Disassemble()
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

void Compiler::ClearCurrentDepthWithPOPInst()
{
    if (symbols.vars.size() == 0)
        return;

    size_t count = 0;

    while (symbols.vars.size() > 0 && symbols.vars.back().depth == symbols.depth)
    {
        size_t varSize = symbols.vars.back().size;
        symbols.ReduceSP(varSize);
        AddCode({Opcode::POP, varSize});
        symbols.vars.pop_back();
        count++;
    }
}

void Compiler::SerialiseProgram(Compiler &prog, std::string fPath)
{
    std::ofstream file;

    if (DoesFileExist(fPath))
        SerialisationError("File '" + fPath + "' already exists, so serialisation would append");

    file.open(fPath, std::ios::out | std::ios::app | std::ios::binary);

    // serialising the index of the 'void Main()' function
    file.write((char *)&prog.main_index, sizeof(prog.main_index));

    // serialising the number of functions
    oprand_t numFunctions = static_cast<oprand_t>(prog.functions.size());
    file.write((char *)&numFunctions, sizeof(numFunctions));

    for (Function &func : prog.functions)
        SerialiseFunction(func, file);

    // serialising the struct tree
    file.write((char *)&STRUCT_TREE_ID, sizeof(STRUCT_TREE_ID));

    // writing the number of structs
    TypeID num_structs = static_cast<TypeID>(prog.struct_tree.size());
    file.write((char *)&num_structs, sizeof(num_structs));

    // writing: struct id, number of parents, parent ids for each struct
    for (auto &s : prog.struct_tree)
    {
        file.write((char *)&s.first, sizeof(s.first));

        size_t num_parents = s.second.size();
        file.write((char *)&num_parents, sizeof(num_parents));

        for (auto &parent : s.second)
            file.write((char *)&parent, sizeof(parent));
    }

    size_t lib_funcID = LIB_FUNC_ID;
    file.write((char *)&lib_funcID, sizeof(LIB_FUNC_ID));

    size_t num_lib_funcs = prog.lib_funcs.size();
    file.write((char *)&num_lib_funcs, sizeof(num_lib_funcs));

    for (auto &lib_func : prog.lib_funcs)
    {
        // writing the name of the function
        size_t name_len = lib_func.name.length();
        file.write((char *)&name_len, sizeof(name_len));
        SerialiseData(&lib_func.name[0], sizeof(char), name_len, file);

        // writing the library
        size_t lib_len = lib_func.library.length();
        file.write((char *)&lib_len, sizeof(lib_len));
        SerialiseData(&lib_func.library[0], sizeof(char), lib_len, file);

        // writing the arity of the library function
        file.write((char *)&lib_func.arity, sizeof(lib_func.arity));
    }

    SerialiseThrowInfo(prog.throw_stack, file);
    file.close();
}

void Compiler::SerialisationError(std::string err)
{
    Error e = Error("[SERIALISATION ERROR]\n" + err + "\n");
    throw e;
}

bool Compiler::DoesFileExist(std::string &path)
{
    return std::ifstream(path).good();
}

void Compiler::SerialiseFunction(Function &f, std::ofstream &file)
{
    // write arity
    file.write((char *)&f.arity, sizeof(f.arity));

    // write constants and code
    SerialiseInts(f, file);
    SerialiseDoubles(f, file);
    SerialiseBools(f, file);
    SerialiseChars(f, file);
    SerialiseStrings(f, file);
    SerialiseOps(f, file);
}

// private:
//=================================SERIALISATION=================================//
void Compiler::SerialiseData(void *data, size_t type_size, size_t num_elements, std::ofstream &file)
{
    file.write((char *)&num_elements, sizeof(num_elements));
    file.write((char *)data, type_size * num_elements);
}

void Compiler::SerialiseInts(Function &f, std::ofstream &file)
{
    file.write((char *)&INT_ID, sizeof(INT_ID));
    SerialiseData(f.ints.data(), sizeof(int), f.ints.size(), file);
}

void Compiler::SerialiseDoubles(Function &f, std::ofstream &file)
{
    file.write((char *)&DOUBLE_ID, sizeof(DOUBLE_ID));
    SerialiseData(f.doubles.data(), sizeof(double), f.doubles.size(), file);
}

void Compiler::SerialiseBools(Function &f, std::ofstream &file)
{
    file.write((char *)&BOOL_ID, sizeof(BOOL_ID));
    bool *data = new bool[f.bools.size()];
    std::copy(std::begin(f.bools), std::end(f.bools), data);
    SerialiseData(data, sizeof(bool), f.bools.size(), file);
    delete[] data;
}

void Compiler::SerialiseChars(Function &f, std::ofstream &file)
{
    file.write((char *)&CHAR_ID, sizeof(CHAR_ID));
    SerialiseData(f.chars.data(), sizeof(char), f.chars.size(), file);
}

void Compiler::SerialiseStrings(Function &f, std::ofstream &file)
{
    file.write((char *)&STRING_ID, sizeof(STRING_ID));
    size_t num_strings = f.strings.size();
    file.write((char *)&num_strings, sizeof(num_strings));

    for (std::string &str : f.strings)
        SerialiseData(&str[0], sizeof(char), str.length(), file);
}

void Compiler::SerialiseOps(Function &f, std::ofstream &file)
{
    file.write((char *)&CODE_ID, sizeof(INT_ID));

    size_t routine_size = f.routines.size();
    file.write((char *)&routine_size, sizeof(routine_size));
    for (auto &routine : f.routines)
    {
        size_t num_ops = routine.size();
        file.write((char *)&num_ops, sizeof(num_ops));

        for (auto &op : routine)
        {
            op_t code_as_num = static_cast<op_t>(op.code);
            file.write((char *)&code_as_num, sizeof(code_as_num));
            file.write((char *)&op.op, sizeof(op.op));
        }
    }
}

void Compiler::SerialiseThrowInfo(std::vector<ThrowInfo> &infos, std::ofstream &file)
{
    file.write((char *)&THROW_INFO_ID, sizeof(INT_ID));

    size_t num_throws = infos.size();
    SerialiseData(&num_throws, 0, num_throws, file);

    for (auto &ti : infos)
    {
        file.write((char *)&ti.is_array, sizeof(ti.is_array));
        file.write((char *)&ti.type, sizeof(ti.type));
        file.write((char *)&ti.func, sizeof(ti.func));
        file.write((char *)&ti.index, sizeof(ti.index));
    }
}
