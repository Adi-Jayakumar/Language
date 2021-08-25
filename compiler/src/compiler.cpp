#include "compiler.h"

Compiler::Compiler()
{
    Functions.push_back(Function());
    Functions[0].arity = 0;
    cur = &Functions[0];

    routineStack.push_back(&cur->routines[0]);
}

void Compiler::CompileError(Token loc, std::string err)
{
    Error e = Error("[COMPILE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void Compiler::TypeError(Token loc, std::string err)
{
    Error e = Error("[TYPE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void Compiler::SymbolError(Token loc, std::string err)
{
    Error e = Error("[SYMBOL ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void Compiler::AddCode(Op o)
{
    routineStack.back()->push_back(o);
}

size_t Compiler::CodeSize()
{
    return routineStack.back()->size();
}

void Compiler::AddRoutine()
{
    cur->routines.push_back(std::vector<Op>());
    routineStack.push_back(&cur->routines.back());
}

void Compiler::RemoveRoutine()
{
    routineStack.pop_back();
}

size_t Compiler::GetVariableStackLoc(std::string &name)
{
    return Symbols.GetVariableStackLoc(name);
}

void Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    mainIndex = MAX_OPRAND;
    size_t numFunctions = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        s[i]->NodeCompile(*this);
        if (dynamic_cast<FuncDecl *>(s[i].get()) != nullptr)
        {
            numFunctions++;
            FuncDecl *asFD = static_cast<FuncDecl *>(s[i].get());
            if (asFD->argtypes.size() == 0 && asFD->name == "Main")
            {
                if (mainIndex != MAX_OPRAND)
                    CompileError(asFD->Loc(), "Main function already defined");
                mainIndex = numFunctions;
            }
        }
        else if (dynamic_cast<DeclaredVar *>(s[i].get()) == nullptr && dynamic_cast<StructDecl *>(s[i].get()) == nullptr && dynamic_cast<ImportStmt *>(s[i].get()) == nullptr)
            CompileError(s[i]->Loc(), "Only declarations allowed in global region");
    }
}

void Compiler::Disassemble()
{
    for (size_t i = 0; i < Functions.size(); i++)
    {
        std::cout << "Function index: " << i << std::endl
                  << "Function arity: " << +Functions[i].arity
                  << std::endl
                  << std::endl;

        Functions[i].PrintCode();

        std::cout << std::endl
                  << std::endl;
    }
}

void Compiler::ClearCurrentDepthWithPOPInst()
{
    while (Symbols.vars.size() > 0 && Symbols.vars.back().depth == Symbols.depth)
    {
        Symbols.vars.pop_back();
        AddCode({Opcode::POP, 0});
    }
}

void Compiler::SerialiseProgram(Compiler &prog, std::string fPath)
{
    std::ofstream file;

    if (DoesFileExist(fPath))
        SerialisationError("File '" + fPath + "' already exists, so serialisation would append");

    file.open(fPath, std::ios::out | std::ios::app | std::ios::binary);

    // serialising the index of the 'void Main()' function
    file.write((char *)&prog.mainIndex, sizeof(prog.mainIndex));

    // serialising the number of functions
    oprand_t numFunctions = static_cast<oprand_t>(prog.Functions.size());
    file.write((char *)&numFunctions, sizeof(numFunctions));

    for (Function &func : prog.Functions)
        SerialiseFunction(func, file);

    // serialising the struct tree
    file.write((char *)&STRUCT_TREE_ID, sizeof(STRUCT_TREE_ID));

    // writing the number of structs
    TypeID numStructs = static_cast<TypeID>(prog.StructTree.size());
    file.write((char *)&numStructs, sizeof(numStructs));

    // writing: struct id, number of parents, parent ids for each struct
    for (auto &s : prog.StructTree)
    {
        file.write((char *)&s.first, sizeof(s.first));

        size_t numParents = s.second.size();
        file.write((char *)&numParents, sizeof(numParents));

        for (auto &parent : s.second)
            file.write((char *)&parent, sizeof(parent));
    }

    size_t libFuncID = LIB_FUNC_ID;
    file.write((char *)&libFuncID, sizeof(LIB_FUNC_ID));

    size_t numLibFuncs = prog.libfuncs.size();
    file.write((char *)&numLibFuncs, sizeof(numLibFuncs));

    for (auto &libfunc : prog.libfuncs)
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

    SerialiseThrowInfo(prog.throwStack, file);
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
