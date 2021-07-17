#include "compiler.h"

Compiler::Compiler()
{
    Functions.push_back(Function());
    Functions[0].arity = 0;
    cur = &Functions[0];
}

void Compiler::CompileError(Token loc, std::string err)
{
    Error e = Error("[COMPILE ERROR] On line " + std::to_string(loc.line) + " near '" + loc.literal + "'\n" + err + "\n");
    throw e;
}

void Compiler::Compile(std::vector<std::shared_ptr<Stmt>> &s)
{
    mainIndex = SIZE_MAX;
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
                if (mainIndex != SIZE_MAX)
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
    std::cout << "NUM FunctionS: " << Functions.size() << std::endl
              << std::endl
              << std::endl;
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
        cur->code.push_back({Opcode::POP, 0});
    }
}

void Compiler::SerialiseProgram(Compiler &prog, std::string fPath)
{
    std::ofstream file;

    if (DoesFileExist(fPath))
        SerialisationError("File '" + fPath + "' already exists, so serialisation would append");

    file.open(fPath, std::ios::out | std::ios::app | std::ios::binary);

    // serialising the index of the 'void Main()' function
    uint8_t mainIndex = static_cast<uint8_t>(prog.mainIndex);
    file.write((char *)&mainIndex, sizeof(uint8_t));

    // serialising the number of functions
    uint8_t numFunctions = static_cast<uint8_t>(prog.Functions.size());
    file.write((char *)&numFunctions, sizeof(uint8_t));

    for (Function &func : prog.Functions)
        SerialiseFunction(func, file);

    // serialising the struct tree
    size_t structTreeID = STRUCT_TREE_ID;
    file.write((char *)&structTreeID, sizeof(size_t));

    // writing the number of structs
    size_t numStructs = prog.StructTree.size();
    file.write((char *)&numStructs, sizeof(size_t));

    for (auto &s : prog.StructTree)
    {
        // writing the struct id
        file.write((char *)&s.first, sizeof(size_t));

        size_t numParents = s.second.size();
        // writing the number of parents
        file.write((char *)&numParents, sizeof(size_t));
        // writing the parent ids
        for (auto &parent : s.second)
            file.write((char *)&parent, sizeof(size_t));
    }

    size_t libFuncID = LIB_FUNC_ID;
    file.write((char *)&libFuncID, sizeof(size_t));

    size_t numLibFuncs = prog.libfuncs.size();
    file.write((char *)&numLibFuncs, sizeof(size_t));

    for (auto &libfunc : prog.libfuncs)
    {
        // writing the name of the function
        size_t nameLen = libfunc.name.length();
        file.write((char *)&nameLen, sizeof(size_t));
        SerialiseData(&libfunc.name[0], sizeof(char), nameLen, file);

        // writing the library
        size_t libLen = libfunc.library.length();
        file.write((char *)&libLen, sizeof(size_t));
        SerialiseData(&libfunc.library[0], sizeof(char), libLen, file);

        // writing the arity of the library function
        file.write((char *)&libfunc.arity, sizeof(size_t));
    }

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
    file.write((char *)&f.arity, sizeof(uint8_t));

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
    // write the number of elements
    file.write(reinterpret_cast<char *>(&numElements), sizeof(size_t));
    // then the elements themselves
    file.write(reinterpret_cast<char *>(data), typeSize * numElements);
}

void Compiler::SerialiseInts(Function &f, std::ofstream &file)
{
    size_t id = INT_ID;
    SerialiseData(&id, 0, id, file);

    SerialiseData(f.ints.data(), sizeof(int), f.ints.size(), file);
}

void Compiler::SerialiseDoubles(Function &f, std::ofstream &file)
{
    size_t id = DOUBLE_ID;
    SerialiseData(&id, 0, id, file);

    SerialiseData(f.doubles.data(), sizeof(double), f.doubles.size(), file);
}

void Compiler::SerialiseBools(Function &f, std::ofstream &file)
{
    size_t id = BOOL_ID;
    SerialiseData(&id, 0, id, file);

    bool *data = new bool[f.bools.size()];
    std::copy(std::begin(f.bools), std::end(f.bools), data);
    SerialiseData(data, sizeof(bool), f.bools.size(), file);

    delete[] data;
}

void Compiler::SerialiseChars(Function &f, std::ofstream &file)
{
    size_t id = CHAR_ID;
    SerialiseData(&id, 0, id, file);
    SerialiseData(f.chars.data(), sizeof(char), f.chars.size(), file);
}

void Compiler::SerialiseStrings(Function &f, std::ofstream &file)
{
    size_t id = STRING_ID;
    SerialiseData(&id, 0, id, file);

    size_t numStrings = f.strings.size();
    // writing the number of strings to the file once
    SerialiseData(&numStrings, 0, numStrings, file);

    for (std::string &str : f.strings)
        SerialiseData(&str[0], sizeof(char), str.length(), file);
}

void Compiler::SerialiseOps(Function &f, std::ofstream &file)
{
    size_t id = CODE_ID;
    SerialiseData(&id, 0, id, file);

    size_t numOps = f.code.size();
    SerialiseData(&numOps, 0, numOps, file);

    for (auto &op : f.code)
    {
        uint8_t codeAsNum = static_cast<uint8_t>(op.code);
        file.write(reinterpret_cast<char *>(&codeAsNum), sizeof(codeAsNum));

        file.write(reinterpret_cast<char *>(&op.op), sizeof(op.op));
    }
}

void Compiler::SerialiseThrowInfo(std::vector<ThrowInfo> &infos, std::ofstream &file)
{
    size_t id = THROW_INFO_ID;
    SerialiseData(&id, 0, id, file);

    size_t numThrows = infos.size();
    SerialiseData(&numThrows, 0, numThrows, file);

    for (auto &ti : infos)
    {
        file.write((char *)&ti.isArray, sizeof(bool));
        file.write((char *)&ti.type, sizeof(uint8_t));
        file.write((char *)&ti.func, sizeof(uint8_t));
        file.write((char *)&ti.index, sizeof(uint8_t));
    }
}
