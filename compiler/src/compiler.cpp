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
    size_t numFunctions = 0;
    for (parse_index = 0; parse_index < s.size(); parse_index++)
    {
        s[parse_index]->NodeCompile(*this);
        if (dynamic_cast<FuncDecl *>(s[parse_index].get()) != nullptr)
        {
            numFunctions++;
            FuncDecl *asFD = static_cast<FuncDecl *>(s[parse_index].get());
            if (asFD->params.size() == 0 && asFD->name == "Main")
            {
                if (main_index != MAX_OPRAND)
                    CompileError(asFD->Loc(), "Main function already defined");
                main_index = numFunctions;
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
    TypeID numStructs = static_cast<TypeID>(prog.struct_tree.size());
    file.write((char *)&numStructs, sizeof(numStructs));

    // writing: struct id, number of parents, parent ids for each struct
    for (auto &s : prog.struct_tree)
    {
        file.write((char *)&s.first, sizeof(s.first));

        size_t numParents = s.second.size();
        file.write((char *)&numParents, sizeof(numParents));

        for (auto &parent : s.second)
            file.write((char *)&parent, sizeof(parent));
    }

    size_t libFuncID = LIB_FUNC_ID;
    file.write((char *)&libFuncID, sizeof(LIB_FUNC_ID));

    size_t numLibFuncs = prog.lib_funcs.size();
    file.write((char *)&numLibFuncs, sizeof(numLibFuncs));

    for (auto &libfunc : prog.lib_funcs)
    {
        // writing the name of the function
        size_t nameLen = libfunc.name.length();
        file.write((char *)&nameLen, sizeof(nameLen));
        SerialiseData(&libfunc.name[0], sizeof(char), nameLen, file);

        // writing the library
        size_t libLen = libfunc.library.length();
        file.write((char *)&libLen, sizeof(libLen));
        SerialiseData(&libfunc.library[0], sizeof(char), libLen, file);

        // writing the arity of the library function
        file.write((char *)&libfunc.arity, sizeof(libfunc.arity));
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
void Compiler::SerialiseData(void *data, size_t typeSize, size_t numElements, std::ofstream &file)
{
    file.write((char *)&numElements, sizeof(numElements));
    file.write((char *)data, typeSize * numElements);
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
    size_t numStrings = f.strings.size();
    file.write((char *)&numStrings, sizeof(numStrings));

    for (std::string &str : f.strings)
        SerialiseData(&str[0], sizeof(char), str.length(), file);
}

void Compiler::SerialiseOps(Function &f, std::ofstream &file)
{
    file.write((char *)&CODE_ID, sizeof(INT_ID));

    size_t routineSize = f.routines.size();
    file.write((char *)&routineSize, sizeof(routineSize));
    for (auto &routine : f.routines)
    {
        size_t numOps = routine.size();
        file.write((char *)&numOps, sizeof(numOps));

        for (auto &op : routine)
        {
            op_t codeAsNum = static_cast<op_t>(op.code);
            file.write((char *)&codeAsNum, sizeof(codeAsNum));
            file.write((char *)&op.op, sizeof(op.op));
        }
    }
}

void Compiler::SerialiseThrowInfo(std::vector<ThrowInfo> &infos, std::ofstream &file)
{
    file.write((char *)&THROW_INFO_ID, sizeof(INT_ID));

    size_t numThrows = infos.size();
    SerialiseData(&numThrows, 0, numThrows, file);

    for (auto &ti : infos)
    {
        file.write((char *)&ti.isArray, sizeof(ti.isArray));
        file.write((char *)&ti.type, sizeof(ti.type));
        file.write((char *)&ti.func, sizeof(ti.func));
        file.write((char *)&ti.index, sizeof(ti.index));
    }
}
