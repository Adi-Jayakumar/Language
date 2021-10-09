#include "vm.h"

VM::VM(std::vector<Function> &_functions, oprand_t mainIndex,
       std::unordered_map<oprand_t, std::unordered_set<oprand_t>> &_StructTree,
       std::vector<LibraryFunctionDef> &_syms,
       std::vector<ThrowInfo> &_throwInfos)
{
    functions = _functions;
    StructTree = _StructTree;
    throwInfos = _throwInfos;

    // for (auto &lf : _syms)
    // {
    //     std::string libpath = "./lib/lib" + lf.library + ".so";
    //     void *handle = dlopen(libpath.c_str(), RTLD_NOW);
    //     libHandles.push_back(handle);

    //     LibFunc func;
    //     *(void **)&func = dlsym(handle, lf.name.c_str());
    //     CLibs.push_back({func, lf.arity});
    // }

    curFunc = mainIndex == UINT8_MAX ? UINT8_MAX : 0;
    cs.push_back({0, 0, 0});
    cs.push_back({0, mainIndex, 0});
    curCF = &cs.back();

    ip = 0;
    curRoutine = 0;
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
        std::cout << "(" << cf.retIndex << ", " << cf.retFunction << ", " << cf.valStackMin << ")" << std::endl;
}

void VM::RuntimeError(std::string msg)
{
    std::cout << "[RUNTIME ERROR] " << msg << std::endl;
    exit(4);
}

void VM::Jump(size_t jump)
{
    ip += jump;
}

void VM::ExecuteProgram()
{
    if (curFunc == UINT8_MAX)
        return;
    while (true)
    {
        while (ip < functions[curFunc].routines[curRoutine].size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        if (!cs.empty())
        {

            CallFrame returnCF = *curCF;
            cs.pop_back();
            curCF = &cs.back();
            ip = returnCF.retIndex;

            if (curFunc != 0)
                ip++;

            curFunc = returnCF.retFunction;
            curRoutine = 0;
            ip = 0;
            size_t stackDiff = stack.count - returnCF.valStackMin;

            // cleaning up the function's constants
            stack.count -= stackDiff;
            stack.back = stack[stack.count];
        }
        else
            return;
    }
}

#define BINARY_I_OP(l, op, r) \
    NewInt(GetInt(l) op GetInt(r))

#define BINARY_DI_OP(l, op, r) \
    NewDouble(GetDouble(l) op GetInt(r))

#define BINARY_ID_OP(l, op, r) \
    NewDouble(GetInt(l) op GetDouble(r))

#define BINARY_D_OP(l, op, r) \
    NewDouble(GetDouble(l) op GetDouble(r))

#define UNARY_I_OP(op, r) \
    NewInt(op GetInt(r))

#define UNARY_D_OP(op, r) \
    NewDouble(op GetDouble(r))

#define UNARY_B_OP(op, r) \
    NewBool(op r->as.b)

#define TAKE_LEFT_RIGHT(left, right, stack) \
    right = stack.back;                     \
    stack.pop_back();                       \
    left = stack.back;                      \
    stack.pop_back()

void VM::ExecuteInstruction()
{
    Op o = functions[curFunc].routines[curRoutine][ip];
    switch (o.code)
    {
    case Opcode::POP:
    {
        stack.pop_back();
        break;
    }
    case Opcode::LOAD_INT:
    {
        break;
    }
    case Opcode::LOAD_DOUBLE:
    {
        break;
    }
    case Opcode::LOAD_BOOL:
    {
        break;
    }
    case Opcode::LOAD_STRING:
    {
        break;
    }
    case Opcode::LOAD_CHAR:
    {
        break;
    }
    case Opcode::VAR_A_GLOBAL:
    {
        // globals[o.op] = stack.back;
        break;
    }
    case Opcode::VAR_D_GLOBAL:
    {
        // globals.push_back(stack.back);
        break;
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
        curRoutine = o.op;
        ip = -1;
        break;
    }
    case Opcode::GOTO_LABEL_IF_FALSE:
    {
        break;
    }
    case Opcode::CALL_F:
    {
        cs.push_back({ip, curFunc, stack.count - functions[o.op].arity});
        curCF = &cs.back();

        if (cs.size() > STACK_MAX)
            RuntimeError("CallStack overflow. Used: " + std::to_string(cs.size()) + " call-frams");

        curFunc = o.op;
        ip = -1;
        break;
    }
    case Opcode::CALL_LIBRARY_FUNC:
    {
        break;
    }
    case Opcode::RETURN:
    {
        CallFrame returnCF = *curCF;
        cs.pop_back();
        curCF = &cs.back();

        ip = returnCF.retIndex;
        curFunc = returnCF.retFunction;

        size_t stackDiff = stack.count - returnCF.valStackMin;
        // Object *retVal = stack.back;

        // cleaning up the function's constants
        stack.pop_N(stackDiff);
        // stack.push_back(retVal);
        break;
    }
    case Opcode::RETURN_VOID:
    {
        CallFrame returnCF = *curCF;
        cs.pop_back();
        curCF = &cs.back();

        ip = returnCF.retIndex;
        curFunc = returnCF.retFunction;

        size_t stackDiff = stack.count - returnCF.valStackMin;

        // cleaning up the function's constants
        stack.pop_N(stackDiff);
        break;
    }
    case Opcode::PUSH_THROW_INFO:
    {
        ThrowStack.push(throwInfos[o.op]);
        break;
    }
    case Opcode::THROW:
    {
        break;
    }
    case Opcode::NATIVE_CALL:
    {
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
        break;
    }
    case Opcode::DI_ADD:
    {
        break;
    }
    case Opcode::ID_ADD:
    {
        break;
    }
    case Opcode::D_ADD:
    {
        break;
    }
    case Opcode::S_ADD:
    {
        break;
    }
    // SUBTRACTIONS: subtracts the last 2 things on the stack
    // if o.op is 1 then is a unary negation (only the case
    // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        break;
    }
    case Opcode::D_SUB:
    {
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        break;
    }
    case Opcode::DI_MUL:
    {
        break;
    }
    case Opcode::ID_MUL:
    {
        break;
    }
    case Opcode::D_MUL:
    {
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        break;
    }
    case Opcode::DI_DIV:
    {
        break;
    }
    case Opcode::ID_DIV:
    {
        break;
    }
    case Opcode::D_DIV:
    {
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        break;
    }
    case Opcode::DI_GT:
    {
        break;
    }
    case Opcode::ID_GT:
    {
        break;
    }
    case Opcode::D_GT:
    {
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        break;
    }
    case Opcode::DI_LT:
    {
        break;
    }
    case Opcode::ID_LT:
    {
        break;
    }
    case Opcode::D_LT:
    {
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        break;
    }
    case Opcode::DI_GEQ:
    {
        break;
    }
    case Opcode::ID_GEQ:
    {
        break;
    }
    case Opcode::D_GEQ:
    {
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        break;
    }
    case Opcode::DI_LEQ:
    {
        break;
    }
    case Opcode::ID_LEQ:
    {
        break;
    }
    case Opcode::D_LEQ:
    {
        break;
    }
    case Opcode::N_EQ_EQ:
    {
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        break;
    }
    case Opcode::D_EQ_EQ:
    {
        break;
    }
    case Opcode::B_EQ_EQ:
    {
        break;
    }
    case Opcode::N_BANG_EQ:
    {
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        break;
    }
    case Opcode::D_BANG_EQ:
    {
        break;
    }
    case Opcode::B_BANG_EQ:
    {
        break;
    }
    case Opcode::B_AND_AND:
    {
        break;
    }
    case Opcode::B_OR_OR:
    {
        break;
    }
    case Opcode::BANG:
    {
        break;
    }
    // Does nothing
    case Opcode::NONE:
    {
        break;
    }
    }
}

VM VM::DeserialiseProgram(std::string fPath)
{
    std::ifstream file;
    if (!DoesFileExist(fPath))
        DeserialisationError("File '" + fPath + "' does not exist");

    file.open(fPath, std::ios::in | std::ios::binary);

    oprand_t mainIndex;
    file.read((char *)&mainIndex, sizeof(mainIndex));

    oprand_t numFunctions;
    file.read((char *)&numFunctions, sizeof(numFunctions));

    std::vector<Function> program;
    for (oprand_t i = 0; i < numFunctions; i++)
        program.push_back(DeserialiseFunction(file));

    std::unordered_map<oprand_t, std::unordered_set<oprand_t>> StructTree;
    std::vector<LibraryFunctionDef> libFuncs;
    std::vector<ThrowInfo> throwInfos;

    while (file.peek() != EOF)
    {
        size_t id = ReadSizeT(file);
        switch (id)
        {
        case STRUCT_TREE_ID:
        {
            TypeID numStructs;
            file.read((char *)&numStructs, sizeof(numStructs));

            for (size_t i = 0; i < numStructs; i++)
            {
                TypeID structId;
                file.read((char *)&structId, sizeof(structId));

                size_t numParents = ReadSizeT(file);

                for (size_t i = 0; i < numParents; i++)
                {
                    TypeID parent;
                    file.read((char *)&parent, sizeof(parent));
                    StructTree[structId].insert(parent);
                }
            }
            break;
        }
        case LIB_FUNC_ID:
        {
            size_t numLibFuncs = ReadSizeT(file);

            for (size_t i = 0; i < numLibFuncs; i++)
            {
                size_t nameLen = ReadSizeT(file);
                ReadSizeT(file);
                char *cName = (char *)DeserialiseData(nameLen, sizeof(char), file);
                std::string name(cName, nameLen);
                delete[] cName;

                size_t libLen = ReadSizeT(file);
                ReadSizeT(file);
                char *cLibName = (char *)DeserialiseData(libLen, sizeof(char), file);
                std::string libName(cLibName, libLen);
                delete[] cLibName;

                size_t arity = ReadSizeT(file);
                libFuncs.push_back(LibraryFunctionDef(name, libName, arity));
            }
            break;
        }
        case THROW_INFO_ID:
        {
            throwInfos = DeserialiseThrowInfos(file);
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
    return VM(program, mainIndex, StructTree, libFuncs, throwInfos);
}

bool VM::DoesFileExist(std::string &path)
{
    return std::ifstream(path).good();
}

void VM::DeserialisationError(std::string err)
{
    Error e = Error("[DE-SERIALISATION ERROR]\n" + err + "\n");
    throw e;
}

Function VM::DeserialiseFunction(std::ifstream &file)
{
    oprand_t arity;
    file.read((char *)&arity, sizeof(arity));

    std::vector<int> Ints;
    std::vector<double> Doubles;
    std::vector<bool> Bools;
    std::vector<char> Chars;
    std::vector<std::string> Strings;
    std::vector<std::vector<Op>> Code;

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

//=================================DE-SERIALISATION=================================//

size_t VM::ReadSizeT(std::ifstream &file)
{
    char cNumElements[sizeof(size_t)];
    file.read(cNumElements, sizeof(size_t));

    return *(size_t *)cNumElements;
}

void *VM::DeserialiseData(size_t numElements, size_t typeSize, std::ifstream &file)
{
    char *cData = new char[typeSize * numElements];

    file.read(cData, typeSize * numElements);
    return cData;
}

std::vector<int> VM::DeserialiseInts(std::ifstream &file)
{
    size_t numInts = ReadSizeT(file);
    int *data = (int *)DeserialiseData(numInts, sizeof(int), file);

    std::vector<int> result(data, data + numInts);
    delete[] data;
    return result;
}

std::vector<double> VM::DeserialiseDoubles(std::ifstream &file)
{
    size_t numDoubles = ReadSizeT(file);
    double *data = (double *)DeserialiseData(numDoubles, sizeof(double), file);

    std::vector<double> result(data, data + numDoubles);
    delete[] data;
    return result;
}

std::vector<bool> VM::DeserialiseBools(std::ifstream &file)
{
    size_t numBools = ReadSizeT(file);
    bool *data = (bool *)DeserialiseData(numBools, sizeof(bool), file);

    std::vector<bool> result(data, data + numBools);
    delete[] data;
    return result;
}

std::vector<char> VM::DeserialiseChars(std::ifstream &file)
{
    size_t numChars = ReadSizeT(file);
    char *data = (char *)DeserialiseData(numChars, sizeof(char), file);

    std::vector<char> result(data, data + numChars);
    delete[] data;
    return result;
}

std::vector<std::string> VM::DeserialiseStrings(std::ifstream &file)
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

std::vector<std::vector<Op>> VM::DeserialiseOps(std::ifstream &file)
{
    size_t numRoutines = ReadSizeT(file);
    std::vector<std::vector<Op>> result;

    for (size_t j = 0; j < numRoutines; j++)
    {

        size_t numOps = ReadSizeT(file);
        std::vector<Op> routine;

        for (size_t i = 0; i < numOps; i++)
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
    size_t numThrows = ReadSizeT(file);
    std::vector<ThrowInfo> result;

    for (size_t i = 0; i < numThrows; i++)
    {
        ThrowInfo ti = ThrowInfo();
        file.read((char *)&ti.isArray, sizeof(bool));
        file.read((char *)&ti.type, sizeof(ti.type));
        file.read((char *)&ti.func, sizeof(ti.func));
        file.read((char *)&ti.index, sizeof(ti.index));
        result.push_back(ti);
    }
    return result;
}