#include "compiler.h"
#include "function.h"
#include "perror.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

struct Serialise
{

    static void SerialisationError(std::string err)
    {
        Error e = Error("[SERIALISATION ERROR]\n" + err + "\n");
        throw e;
    }

    static void DeserialisationError(std::string err)
    {
        Error e = Error("[DE-SERIALISATION ERROR]\n" + err + "\n");
        throw e;
    }

    static bool DoesFileExist(std::string &path)
    {
        return std::ifstream(path).good();
    }

#define INT_ID 0xAAAAAAAAAAAAAAAA
#define DOUBLE_ID 0xBBBBBBBBBBBBBBBB
#define BOOL_ID 0xCCCCCCCCCCCCCCCC
#define CHAR_ID 0xDDDDDDDDDDDDDDDD
#define STRING_ID 0xEEEEEEEEEEEEEEEE
#define CODE_ID 0xFFFFFFFFFFFFFFFF
#define STRUCT_TREE_ID 0xABABABABABABABAB
#define LIB_FUNC_ID 0xBCBCBCBCBCBCBCBC

    static void SerialiseProgram(Compiler &prog, std::string fPath)
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

    static void SerialiseFunction(Function &f, std::ofstream &file)
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

    static std::vector<Function> DeserialiseProgram(std::string fPath)
    {
        std::ifstream file;
        if (!DoesFileExist(fPath))
            DeserialisationError("File '" + fPath + "' does not exist");

        file.open(fPath, std::ios::in | std::ios::binary);

        uint8_t mainIndex;
        file.read((char *)&mainIndex, sizeof(uint8_t));

        uint8_t numFunctions;
        file.read((char *)&numFunctions, sizeof(uint8_t));

        std::vector<Function> program;
        for (uint8_t i = 0; i < numFunctions; i++)
            program.push_back(DeserialiseFunction(file));

        size_t id = ReadSizeT(file);
        switch (id)
        {
        case STRUCT_TREE_ID:
        {
            std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;
            size_t numStructs = ReadSizeT(file);

            for (size_t i = 0; i < numStructs; i++)
            {
                size_t structId = ReadSizeT(file);
                size_t numParents = ReadSizeT(file);

                for (size_t i = 0; i < numParents; i++)
                    StructTree[structId].insert(ReadSizeT(file));
            }

            std::cout << "Struct inheritance tree" << std::endl;

            for (const auto &kv : StructTree)
            {
                std::cout << GetTypeStringMap()[kv.first] << "\t|\t";
                for (const auto &ch : kv.second)
                    std::cout << GetTypeStringMap()[ch] << ", ";
                std::cout << std::endl;
            }

            std::cout << std::endl
                      << std::endl;

            break;
        }
        case LIB_FUNC_ID:
        {
            std::vector<LibraryFunctionDef> libFuncs;
            size_t numLibFuncs = ReadSizeT(file);

            for (size_t i = 0; i < numLibFuncs; i++)
            {
                size_t nameLen = ReadSizeT(file);
                char *cName = (char *)DeserialiseData(nameLen, sizeof(char), file);
                std::string name(cName, nameLen);
                delete[] cName;

                size_t libLen = ReadSizeT(file);
                char *cLibName = (char *)DeserialiseData(libLen, sizeof(char), file);
                std::string libName(cLibName, libLen);
                delete[] cLibName;

                size_t arity = ReadSizeT(file);
                libFuncs.push_back(LibraryFunctionDef(name, libName, arity));
            }
            break;
        }
        default:
        {
            SerialisationError("Invalid section identifier " + std::to_string(id));
            break;
        }
        }

        file.close();
        return program;
    }

    static Function DeserialiseFunction(std::ifstream &file)
    {
        uint8_t arity;
        file.read((char *)&arity, sizeof(uint8_t));

        std::vector<int> Ints;
        std::vector<double> Doubles;
        std::vector<bool> Bools;
        std::vector<char> Chars;
        std::vector<std::string> Strings;
        std::vector<Op> Code;

        for (size_t i = 0; i < 6; i++)
        {
            size_t typeCode = ReadSizeT(file);
            switch (typeCode)
            {
            case INT_ID:
            {
                Ints = DeserialiseInts(file);
                break;
            }
            case DOUBLE_ID:
            {
                Doubles = DeserialiseDoubles(file);
                break;
            }
            case BOOL_ID:
            {
                Bools = DeserialiseBools(file);
                break;
            }
            case CHAR_ID:
            {
                Chars = DeserialiseChars(file);
                break;
            }
            case STRING_ID:
            {
                Strings = DeserialiseStrings(file);
                break;
            }
            case CODE_ID:
            {
                Code = DeserialiseOps(file);
                break;
            }
            }
        }

        return Function(arity, Code, Ints, Doubles, Bools, Chars, Strings);
    }

    // private:
    //=================================SERIALISATION=================================//
    static void SerialiseData(void *data, size_t typeSize, size_t numElements, std::ofstream &file)
    {
        // write the number of elements
        file.write(reinterpret_cast<char *>(&numElements), sizeof(size_t));
        // then the elements themselves
        file.write(reinterpret_cast<char *>(data), typeSize * numElements);
    }

    static void SerialiseInts(Function &f, std::ofstream &file)
    {
        size_t id = INT_ID;
        SerialiseData(&id, 0, id, file);

        SerialiseData(f.ints.data(), sizeof(int), f.ints.size(), file);
    }

    static void SerialiseDoubles(Function &f, std::ofstream &file)
    {
        size_t id = DOUBLE_ID;
        SerialiseData(&id, 0, id, file);

        SerialiseData(f.doubles.data(), sizeof(double), f.doubles.size(), file);
    }

    static void SerialiseBools(Function &f, std::ofstream &file)
    {
        size_t id = BOOL_ID;
        SerialiseData(&id, 0, id, file);

        bool *data = new bool[f.bools.size()];
        std::copy(std::begin(f.bools), std::end(f.bools), data);
        SerialiseData(data, sizeof(bool), f.bools.size(), file);

        delete[] data;
    }

    static void SerialiseChars(Function &f, std::ofstream &file)
    {
        size_t id = CHAR_ID;
        SerialiseData(&id, 0, id, file);
        SerialiseData(f.chars.data(), sizeof(char), f.chars.size(), file);
    }

    static void SerialiseStrings(Function &f, std::ofstream &file)
    {
        size_t id = STRING_ID;
        SerialiseData(&id, 0, id, file);

        size_t numStrings = f.strings.size();
        // writing the number of strings to the file once
        SerialiseData(&numStrings, 0, numStrings, file);

        for (std::string &str : f.strings)
            SerialiseData(&str[0], sizeof(char), str.length(), file);
    }

    static void SerialiseOps(Function &f, std::ofstream &file)
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

#undef INT_ID
#undef DOUBLE_ID
#undef BOOL_ID
#undef CHAR_ID
#undef STRING_ID
#undef CODE_ID

    //=================================DE-SERIALISATION=================================//

    static size_t ReadSizeT(std::ifstream &file)
    {
        char cNumElements[sizeof(size_t)];
        file.read(cNumElements, sizeof(size_t));

        return *(size_t *)cNumElements;
    }

    static void *DeserialiseData(size_t numElements, size_t typeSize, std::ifstream &file)
    {
        char *cData = new char[typeSize * numElements];

        file.read(cData, typeSize * numElements);
        return cData;
    }

    static std::vector<int> DeserialiseInts(std::ifstream &file)
    {
        size_t numInts = ReadSizeT(file);
        int *data = (int *)DeserialiseData(numInts, sizeof(int), file);

        std::vector<int> result(data, data + numInts);
        delete[] data;
        return result;
    }

    static std::vector<double> DeserialiseDoubles(std::ifstream &file)
    {
        size_t numDoubles = ReadSizeT(file);
        double *data = (double *)DeserialiseData(numDoubles, sizeof(double), file);

        std::vector<double> result(data, data + numDoubles);
        delete[] data;
        return result;
    }

    static std::vector<bool> DeserialiseBools(std::ifstream &file)
    {
        size_t numBools = ReadSizeT(file);
        bool *data = (bool *)DeserialiseData(numBools, sizeof(bool), file);

        std::vector<bool> result(data, data + numBools);
        delete[] data;
        return result;
    }

    static std::vector<char> DeserialiseChars(std::ifstream &file)
    {
        size_t numChars = ReadSizeT(file);
        char *data = (char *)DeserialiseData(numChars, sizeof(char), file);

        std::vector<char> result(data, data + numChars);
        delete[] data;
        return result;
    }

    static std::vector<std::string> DeserialiseStrings(std::ifstream &file)
    {
        size_t numStrings = ReadSizeT(file);
        std::vector<std::string> result;

        for (size_t i = 0; i < numStrings; i++)
        {
            size_t strLen = ReadSizeT(file);
            char *data = (char *)DeserialiseData(strLen, sizeof(char), file);
            result.push_back(std::string(data, strLen));
            delete[] data;
        }

        return result;
    }

    static std::vector<Op> DeserialiseOps(std::ifstream &file)
    {
        size_t numOps = ReadSizeT(file);
        std::vector<Op> result;

        for (size_t i = 0; i < numOps; i++)
        {
            void *vCode = DeserialiseData(1, sizeof(Opcode), file);
            Opcode code = static_cast<Opcode>(*(uint8_t *)vCode);
            delete[](char *) vCode;

            void *vOprand = DeserialiseData(1, sizeof(uint8_t), file);
            uint8_t oprand = static_cast<uint8_t>(*(uint8_t *)vOprand);
            delete[](char *) vOprand;

            result.push_back(Op(code, oprand));
        }

        return result;
    }
};