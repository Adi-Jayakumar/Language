#include "vm.h"

// /*
//     ARR_D
//     ARR_ALLOC
//     STRUCT_ALLOC
//     STRUCT_D
// */

VM::VM(std::vector<RuntimeFunction> &_functions, size_t mainIndex, std::unordered_map<size_t, std::unordered_set<size_t>> &_StructTree, std::unordered_map<TypeID, std::string> &_TypeNameMap)
{
    functions = _functions;
    StructTree = _StructTree;
    TypeNameMap = _TypeNameMap;

    cs = new CallFrame[STACK_MAX];
    curCF = cs;
    curFunc = mainIndex == SIZE_MAX ? SIZE_MAX : 0;
    *curCF = {0, mainIndex, 0};
    curCF++;
    *curCF = {0, mainIndex, 0};

    ip = 0;
}

VM::~VM()
{
    delete[] cs;
    for (RuntimeFunction &rtf : functions)
    {
        for (Object *rto : rtf.values)
            std::cout << "value " << rto->ToString() << std::endl;

        std::cout << std::endl
                  << std::endl;
    }

    std::cout << "====================HEAP====================" << std::endl
              << std::endl
              << std::endl;

    for (size_t i = 0; i < Heap.size(); i++)
        std::cout << Heap[i]->ToString() << std::endl;

    std::cout << std::endl
              << std::endl;
}

void VM::PrintStack()
{
    std::cout << "index\t|\tvalue" << std::endl;
    for (size_t i = stack.count - 1; (int)i >= 0; i--)
        std::cout << i << "\t|\t" << stack[i]->ToString() << std::endl;
}

void VM::PrintValues()
{
    for (size_t i = 0; i < functions.size(); i++)
    {
        std::cout << "Values of function " << i << std::endl;

        for (Object *c : functions[i].values)
            std::cout << c->ToString() << std::endl;

        std::cout << std::endl;
    }
}

// Object *VM::Allocate(size_t size)
// {
//     Object *alloc = new Object[size];
//     Heap.push_back(alloc);
//     return alloc;
// }

char *VM::StringAllocate(size_t size)
{
    return new char[size * sizeof(char)];
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
    if (curFunc == SIZE_MAX)
        return;
    while (curCF != cs - 1)
    {
        while (ip < functions[curFunc].code.size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        if (curFunc != 0)
            ip++;

        if (curCF != cs - 1)
        {
            curFunc = returnCF->retChunk;
            size_t stackDiff = stack.count - returnCF->valStackMin;

            // cleaning up the function's constants
            stack.count -= stackDiff;
            stack.back = stack[stack.count];
        }
    }
}

#define BINARY_I_OP(l, op, r) \
    CreateInt(GetInt(l) op GetInt(r))

#define BINARY_DI_OP(l, op, r) \
    CreateDouble(GetDouble(l) op GetInt(r))

#define BINARY_ID_OP(l, op, r) \
    CreateDouble(GetInt(l) op GetDouble(r))

#define BINARY_D_OP(l, op, r) \
    CreateDouble(GetDouble(l) op GetDouble(r))

#define UNARY_I_OP(op, r) \
    CreateInt(op GetInt(r))

#define UNARY_D_OP(op, r) \
    CreateDouble(op GetDouble(r))

#define UNARY_B_OP(op, r) \
    CreateBool(op r->as.b)

#define TAKE_LEFT_RIGHT(left, right, stack) \
    right = stack.back;                     \
    stack.pop_back();                       \
    left = stack.back;                      \
    stack.pop_back()

void VM::ExecuteInstruction()
{
    Op o = functions[curFunc].code[ip];
    switch (o.code)
    {
    case Opcode::POP:
    {
        stack.pop_back();
        break;
    }
    case Opcode::LOAD_INT:
    {
        int i = functions[curFunc].ints[o.op];
        stack.push_back(CreateInt(i));
        break;
    }
    case Opcode::LOAD_DOUBLE:
    {
        double d = functions[curFunc].doubles[o.op];
        stack.push_back(CreateDouble(d));
        break;
    }
    case Opcode::LOAD_BOOL:
    {
        bool b = functions[curFunc].bools[o.op];
        stack.push_back(CreateBool(b));
        break;
    }
    case Opcode::LOAD_STRING:
    {
        std::string str = functions[curFunc].strings[o.op];
        char *c = new char[str.length() + 1];
        stack.push_back(CreateString(strcpy(c, str.c_str()), str.length()));
        break;
    }
    case Opcode::LOAD_CHAR:
    {
        char c = functions[curFunc].chars[o.op];
        stack.push_back(CreateChar(c));
        break;
    }
    case Opcode::VAR_A:
    {
        stack.data[o.op + curCF->valStackMin] = stack.back;
        break;
    }
    case Opcode::VAR_A_GLOBAL:
    {
        globals[o.op] = stack.back;
        break;
    }
    case Opcode::VAR_D_GLOBAL:
    {
        globals.push_back(stack.back);
        break;
    }
    case Opcode::GET_V:
    {
        stack.push_back(stack[o.op + curCF->valStackMin]);
        break;
    }
    case Opcode::GET_V_GLOBAL:
    {
        stack.push_back(globals[o.op]);
        break;
    }
    case Opcode::ARR_D:
    {
        Object *array = stack[stack.count - o.op - 1];
        Object **data = GetArray(array);

        for (size_t i = 0; i < o.op; i++)
            data[i] = stack[stack.count - o.op + i];

        stack.pop_N(o.op);
        break;
    }
    case Opcode::ARR_INDEX:
    {
        Object *index = stack.back;
        stack.pop_back();
        Object *obj = stack.back;

        if (IsNull_T(obj))
            RuntimeError("Cannot index into an uninitialised array");

        Object **data = GetArray(obj);
        stack.pop_back();

        size_t size = GetArrayLength(obj);
        int i = GetInt(index);

        if (i >= (int)size || i < 0)
            RuntimeError("Array i " + std::to_string(i) + " out of bounds for array of size " + std::to_string(size));

        stack.push_back(data[i]);
        break;
    }
    case Opcode::ARR_SET:
    {
        Object *index = stack.back;
        stack.pop_back();
        int i = GetInt(index);

        Object *obj = stack.back;
        Object **data = GetArray(obj);
        stack.pop_back();

        size_t size = GetArrayLength(obj);
        if (i >= (int)size || i < 0)
            RuntimeError("Array index " + std::to_string(i) + " out of bounds for array of size " + std::to_string(size));

        data[i] = stack.back;
        break;
    }
    case Opcode::ARR_ALLOC:
    {
        size_t size = o.op;
        Object **data = new Object *[size];
        Object *arr = CreateArray(data, size);

        for (size_t i = 0; i < size; i++)
            data[i] = CreateNull_T();

        stack.push_back(arr);
        break;
    }
    case Opcode::STRUCT_ALLOC:
    {
        size_t size = o.op;
        Object **data = new Object *[size];
        Object *strct = CreateStruct(data, size, 1);

        stack.push_back(strct);
        break;
    }
    case Opcode::STRING_INDEX:
    {
        Object *indexObj = stack.back;
        stack.pop_back();
        Object *obj = stack.back;

        if (IsNull_T(obj))
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        char *str = GetString(obj);

        size_t size = GetStringLen(obj);
        int index = GetInt(indexObj);

        if (index >= (int)size || index < 0)
            RuntimeError("String index " + std::to_string(index) + " out of bounds for array of size " + std::to_string(size));

        stack.push_back(CreateChar(str[index]));
        break;
    }
    case Opcode::STRING_SET:
    {
        Object *index = stack.back;
        stack.pop_back();
        int i = GetInt(index);

        Object *strObj = stack.back;
        stack.pop_back();

        char *str = GetString(strObj);
        size_t strLen = GetStringLen(strObj);

        if (i >= (int)strLen || i < 0)
            RuntimeError("String index " + std::to_string(i) + " out of bounds for string of size " + std::to_string(strLen));

        str[i] = GetChar(stack.back);
        break;
    }
    case Opcode::JUMP_IF_FALSE:
    {
        if (!stack.back->IsTruthy())
            ip += o.op;
        stack.pop_back();
        break;
    }
    case Opcode::JUMP:
    {
        ip += o.op;
        break;
    }
    case Opcode::LOOP:
    {
        ip = o.op;
        break;
    }
    case Opcode::CALL_F:
    {
        curCF++;

        if (curCF == &cs[STACK_MAX - 1])
            RuntimeError("CallStack overflow. Used: " + std::to_string(curCF - &cs[0]) + " callstacks");

        *curCF = {ip, curFunc, stack.count - functions[o.op].arity};

        curFunc = o.op;
        ip = -1;
        break;
    }
    case Opcode::RETURN:
    {
        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        curFunc = returnCF->retChunk;

        size_t stackDiff = stack.count - returnCF->valStackMin;
        Object *retVal;

        if (o.op == 0)
            retVal = stack.back;

        // cleaning up the function's constants
        stack.pop_N(stackDiff);

        if (o.op == 0)
            stack.push_back(retVal);
        break;
    }
    case Opcode::NATIVE_CALL:
    {
        Object *arityAsCC = stack.back;
        stack.pop_back();

        switch (o.op)
        {
        case 0:
        {
            NativePrint(GetInt(arityAsCC));
            break;
        }
        case 1:
        {
            NativeToString(GetInt(arityAsCC));
        }
        default:
            break;
        }
        break;
    }
    case Opcode::PRINT:
    {
        NativePrint(o.op);
        break;
    }
    case Opcode::STRUCT_MEMBER:
    {
        Object *strct = stack.back;
        if (IsNull_T(strct))
            RuntimeError("Cannot access into a null struct");
        stack.pop_back();
        stack.push_back(GetStructMembers(strct)[o.op]);
        break;
    }
    case Opcode::STRUCT_D:
    {
        Object *arr = stack[stack.count - o.op - 1];
        Object **data = GetStructMembers(arr);

        for (size_t i = 0; i < o.op; i++)
            data[i] = stack[stack.count - o.op + i];

        stack.pop_N(o.op);
        break;
    }
    case Opcode::STRUCT_MEMBER_SET:
    {
        Object *strct = stack.back;

        if (IsNull_T(strct))
            RuntimeError("Cannot set a member of a null struct");
        stack.pop_back();
        GetStructMembers(strct)[o.op] = stack.back;
        break;
    }
    case Opcode::CAST:
    {
        Object *obj = stack.back;
        stack.pop_back();
        if (IsNull_T(obj))
            RuntimeError("Cannot cast a null value");
        else if (dynamic_cast<Int *>(obj) != nullptr)
        {
            stack.push_back(CreateDouble((double)GetInt(obj)));
            break;
        }
        else if (dynamic_cast<Double *>(obj) != nullptr)
        {
            stack.push_back(CreateInt((int)GetDouble(obj)));
            break;
        }
        else if (dynamic_cast<Struct *>(obj) != nullptr)
        {
            size_t type = GetStructType(obj);
            if (StructTree[o.op].find(type) == StructTree[o.op].end() && type != o.op)
            {
                stack.pop_back();
                stack.push_back(CreateNull_T());
            }
            break;
        }
        break;
    }
    // ADDITIONS: adds the last 2 things on the stack
    case Opcode::I_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, +, right));
        break;
    }
    case Opcode::S_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        char *lStr = GetString(left);
        char *rStr = GetString(right);

        size_t leftSize = GetStringLen(left);
        size_t rightSize = GetStringLen(right);
        size_t newStrSize = leftSize + rightSize;

        char *concat = StringAllocate(newStrSize + 1);
        strcpy(concat, lStr);

        char *next = concat + leftSize;
        strcpy(next, rStr);

        stack.push_back(CreateString(concat, newStrSize));
        break;
    }
    // SUBTRACTIONS: subtracts the last 2 things on the stack
    // if o.op is 1 then is a unary negation (only the case
    // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        Object *right = stack.back;
        stack.pop_back();
        if (o.op == 0)
        {
            Object *left = stack.back;
            stack.pop_back();

            stack.push_back(CreateInt(GetInt(left) - GetInt(right)));
        }
        else
            stack.push_back(CreateInt(-GetInt(right)));
        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);

        stack.push_back(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);

        stack.push_back(BINARY_ID_OP(left, -, right));
        break;
    }
    case Opcode::D_SUB:
    {
        Object *right = stack.back;
        stack.pop_back();

        if (o.op == 0)
        {
            Object *left = stack.back;
            stack.pop_back();

            stack.push_back(CreateDouble(GetDouble(left) - GetDouble(right)));
        }
        else
            stack.push_back(CreateDouble(-GetDouble(right)));
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, *, right));
        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, *, right));
        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, *, right));
        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, *, right));
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, /, right));
        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, /, right));
        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, /, right));
        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, /, right));
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, >, right));
        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, >, right));
        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, >, right));
        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, >, right));
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, <, right));
        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, <, right));
        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, <, right));
        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, <, right));
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, >=, right));
        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, >=, right));
        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, >=, right));
        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, >=, right));
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, <=, right));
        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, <=, right));
        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, <=, right));
        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, <=, right));
        break;
    }
    case Opcode::N_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(IsNull_T(left) && IsNull_T(right)));
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, ==, right));
        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, ==, right));
        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, ==, right));
        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, ==, right));
        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(GetBool(left) == GetBool(right)));
        break;
    }
    case Opcode::N_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(!IsNull_T(left) || !IsNull_T(right)));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, !=, right));
        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, !=, right));
        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, !=, right));
        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, !=, right));
        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(GetBool(left) != GetBool(right)));
        break;
    }
    case Opcode::BANG:
    {
        Object *right = stack.back;
        stack.pop_back();
        stack.push_back(CreateBool(!GetBool(right)));
        break;
    }
    // Does nothing
    case Opcode::NONE:
    {
        break;
    }
    }
}

void VM::NativePrint(int arity)
{
    for (size_t i = 0; i < (size_t)arity; i++)
        std::cout << stack[stack.count - arity + i]->ToString();

    std::cout << std::endl;
    stack.pop_N((size_t)arity);
}

void VM::NativeToString(int)
{
    std::string str = stack.back->ToString();
    stack.pop_back();
    char *c = new char[str.length() + 1];
    stack.push_back(new String(strcpy(c, str.c_str()), str.length()));
}