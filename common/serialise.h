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

    static void SerialiseProgram(Compiler &prog, std::string fPath)
    {
        std::ofstream file;

        if (DoesFileExist(fPath))
            SerialisationError("File '" + fPath + "' already exists, so serialisation would append");

        file.open(fPath, std::ios::out | std::ios::app | std::ios::binary);

        for (Function &func : prog.Functions)
            SerialiseFunction(func, file);

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

#define INT_ID 0xAAAAAAAAAAAAAAAA
#define DOUBLE_ID 0xBBBBBBBBBBBBBBBB
#define BOOL_ID 0xCCCCCCCCCCCCCCCC
#define CHAR_ID 0xDDDDDDDDDDDDDDDD
#define STRING_ID 0xEEEEEEEEEEEEEEEE
#define CODE_ID 0xFFFFFFFFFFFFFFFF

    static std::vector<Function> DeserialiseProgram(std::string fPath)
    {
        std::ifstream file;
        if (!DoesFileExist(fPath))
            DeserialisationError("File '" + fPath + "' does not exist");

        file.open(fPath, std::ios::in | std::ios::binary);

        std::vector<Function> program;
        while (file.peek() != EOF)
            program.push_back(DeserialiseFunction(file));

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