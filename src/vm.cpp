#include "vm.h"

/*
    ARR_D
    ARR_ALLOC
    STRUCT_ALLOC
    STRUCT_D
*/

VM::VM(std::vector<RuntimeFunction> &_functions, size_t mainIndex, std::unordered_map<size_t, std::unordered_set<size_t>> &_StructTree)
{
    functions = _functions;
    StructTree = _StructTree;
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
        for (RuntimeObject *rto : rtf.values)
        {
            char *val = RTOToString(rto);
            std::cout << "value " << val << " state " << GetGCState(rto) << std::endl;
            free(val);
            GC::FreeObject(rto);
        }
        std::cout << std::endl
                  << std::endl;
    }

    std::cout << "====================HEAP====================" << std::endl
              << std::endl
              << std::endl;

    for (size_t i = 0; i < Heap.count; i++)
    {
        char *heap = RTOToString(Heap[i]);
        std::cout << heap << std::endl;
        free(heap);
    }

    GC::DeallocateHeap(this);

    std::cout << std::endl
              << std::endl;
}

void VM::PrintStack()
{
    std::cout << "index\t|\tvalue" << std::endl;
    for (size_t i = stack.count - 1; (int)i >= 0; i--)
    {
        char *stackVal = RTOToString(stack[i]);
        std::cout << i << "\t|\t" << stackVal << std::endl;
    }
}

void VM::PrintValues()
{
    for (size_t i = 0; i < functions.size(); i++)
    {
        std::cout << "Values of function " << i << std::endl;
        for (RuntimeObject *c : functions[i].values)
        {
            char *funcVal = RTOToString(c);
            std::cout << funcVal << " state " << GetGCState(c) << std::endl;
        }
        std::cout << std::endl;
    }
}

// RuntimeObject *VM::Allocate(size_t size)
// {
// #ifdef GC_STRESS
//     GC::GarbageCollect(this);
// #endif
//     RuntimeObject *alloc = (RuntimeObject *)malloc(size * sizeof(RuntimeObject));
//     Heap.push_back(alloc);
//     return alloc;
// }

char *VM::StringAllocate(size_t size)
{
#ifdef GC_STRESS
    GC::GarbageCollect(this);
#endif
    return (char *)malloc(size * sizeof(char));
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
#ifdef GC_SUPER_STRESS
            GC::GarbageCollect(this);
#endif
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
    case Opcode::GET_C:
    {
        stack.push_back(functions[curFunc].values[o.op]);
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
        RuntimeObject *arr = stack[stack.count - o.op - 1];

        for (size_t i = 0; i < o.op; i++)
            SetIndexOfArray(arr, i, stack[stack.count - o.op + i]);

        stack.pop_N(o.op);
        break;
    }
    case Opcode::ARR_INDEX:
    {
        RuntimeObject *index = stack.back;
        stack.pop_back();
        RuntimeObject *obj = stack.back;

        Array arr = GetArrayOrStruct(obj);

        if (GetType(obj) == RuntimeType::NULL_T)
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        int size = (int)arr.size;

        if (GetInt(index) >= (int)size || GetInt(index) < 0)
            RuntimeError("Array index " + std::to_string(GetInt(index)) + " out of bounds for array of size " + std::to_string(arr.size));

        stack.push_back(arr.data[GetInt(index)]);
        break;
    }
    case Opcode::ARR_SET:
    {
        RuntimeObject *index = stack.back;
        stack.pop_back();
        int i = GetInt(index);

        RuntimeObject *obj = stack.back;
        Array arr = GetArrayOrStruct(obj);
        stack.pop_back();

        size_t arraySize = arr.size;

        if (i >= (int)arraySize || i < 0)
            RuntimeError("Array index " + std::to_string(i) + " out of bounds for array of size " + std::to_string(arraySize));

        arr.data[i] = CopyRTO(stack.back);
        break;
    }
    case Opcode::ARR_ALLOC:
    {
        size_t arraySize = o.op;
        RuntimeObject *arr = CreateArrayOrStruct(RuntimeType::ARRAY, arraySize);

        for (size_t i = 0; i < arraySize; i++)
            SetIndexOfArray(arr, i, GetNull());

        stack.push_back(arr);
        break;
    }
    case Opcode::STRUCT_ALLOC:
    {
        size_t arraySize = o.op;
        RuntimeObject *arr = CreateArrayOrStruct(RuntimeType::STRUCT, arraySize);

        for (size_t i = 0; i < arraySize; i++)
            SetIndexOfArray(arr, i, GetNull());

        stack.push_back(arr);
        break;
    }
    case Opcode::STRING_INDEX:
    {
        RuntimeObject *indexObj = stack.back;
        stack.pop_back();
        RuntimeObject *obj = stack.back;

        if (GetType(obj) == RuntimeType::NULL_T)
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        String str = GetString(obj);
        int size = (int)str.len;
        int index = GetInt(indexObj);

        if (index >= (int)size || index < 0)
            RuntimeError("String index " + std::to_string(index) + " out of bounds for array of size " + std::to_string(str.len));

        stack.push_back(CreateChar(str.data[index]));
        break;
    }
    case Opcode::STRING_SET:
    {
        RuntimeObject *index = stack.back;
        stack.pop_back();
        int i = GetInt(index);

        RuntimeObject *strObj = stack.back;
        stack.pop_back();
        String str = GetString(strObj);

        size_t strSize = str.len;

        if (i >= (int)strSize || i < 0)
            RuntimeError("String index " + std::to_string(i) + " out of bounds for string of size " + std::to_string(strSize));

        str.data[i] = GetChar(stack.back);
        break;
    }
    case Opcode::JUMP_IF_FALSE:
    {
        if (!IsTruthy(stack.back))
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
        RuntimeObject *retVal;

        if (o.op == 0)
            retVal = stack.back;

        // cleaning up the function's constants
        stack.pop_N(stackDiff);

        if (o.op == 0)
            stack.push_back(CopyRTO(retVal));
        break;
    }
    case Opcode::NATIVE_CALL:
    {
        RuntimeObject *arityAsCC = stack.back;
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
        RuntimeObject *strct = stack.back;
        if (GetType(strct) == RuntimeType::NULL_T)
            RuntimeError("Cannot access into a null struct");
        stack.pop_back();
        stack.push_back(GetArrayOrStruct(strct).data[o.op]);
        break;
    }
    case Opcode::STRUCT_D:
    {
        RuntimeObject *arr = stack[stack.count - o.op - 1];

        for (size_t i = 0; i < o.op; i++)
            SetIndexOfArray(arr, i, stack[stack.count - o.op + i]);

        stack.pop_N(o.op);
        break;
    }
    case Opcode::STRUCT_MEMBER_SET:
    {
        RuntimeObject *strct = stack.back;
        if (GetType(strct) == RuntimeType::NULL_T)
            RuntimeError("Cannot set a member of a null struct");
        stack.pop_back();
        GetArrayOrStruct(strct).data[o.op] = stack.back;
        break;
    }
    case Opcode::CAST:
    {
        switch (GetType(stack.back))
        {
        case RuntimeType::NULL_T:
        {
            RuntimeError("Cannot cast a null value");
            break;
        }
        case RuntimeType::INT:
        {
            RuntimeObject *val = stack.back;
            stack.pop_back();
            stack.push_back(CreateDouble((double)GetInt(val)));
            break;
        }
        case RuntimeType::DOUBLE:
        {
            RuntimeObject *val = stack.back;
            stack.pop_back();
            stack.push_back(CreateInt((int)GetDouble(val)));
            break;
        }
        case RuntimeType::STRUCT:
        {
            // size_t type = GetType(stack.back).type;
            // if (StructTree[o.op].find(type) == StructTree[o.op].end() && type != o.op)
            // {
            //     stack.pop_back();
            //     stack.push_back(GetNull());
            // }
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    // ADDITIONS: adds the last 2 things on the stack
    case Opcode::I_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, +, right));
        break;
    }
    case Opcode::S_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        String lStr = GetString(left);
        String rStr = GetString(right);

        size_t newStrSize = lStr.len + rStr.len;

        char *concat = StringAllocate(newStrSize + 1);
        strcpy(concat, lStr.data);

        char *next = concat + lStr.len;
        strcpy(next, rStr.data);

        String concatStr = {newStrSize, concat};
        stack.push_back(CreateString(concatStr));
        break;
    }
    // SUBTRACTIONS: subtracts the last 2 things on the stack
    // if o.op is 1 then is a unary negation (only the case
    // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        RuntimeObject *right = stack.back;
        stack.pop_back();
        if (o.op == 0)
        {
            RuntimeObject *left = stack.back;
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
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);

        stack.push_back(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);

        stack.push_back(BINARY_ID_OP(left, -, right));
        break;
    }
    case Opcode::D_SUB:
    {
        RuntimeObject *right = stack.back;
        stack.pop_back();

        if (o.op == 0)
        {
            RuntimeObject *left = stack.back;
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
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, *, right));
        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, *, right));
        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, *, right));
        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, *, right));
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, /, right));
        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, /, right));
        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, /, right));
        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, /, right));
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, >, right));
        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, >, right));
        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, >, right));
        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, >, right));
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, <, right));
        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, <, right));
        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, <, right));
        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, <, right));
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, >=, right));
        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, >=, right));
        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, >=, right));
        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, >=, right));
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, <=, right));
        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, <=, right));
        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, <=, right));
        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, <=, right));
        break;
    }
    case Opcode::N_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(CreateBool(GetType(left) == RuntimeType::NULL_T && GetType(right) == RuntimeType::NULL_T));
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, ==, right));
        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, ==, right));
        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, ==, right));
        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, ==, right));
        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(CreateBool(GetBool(left) == GetBool(right)));
        break;
    }
    case Opcode::N_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(CreateBool(GetType(left) != RuntimeType::NULL_T && GetType(right) != RuntimeType::NULL_T));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_I_OP(left, !=, right));
        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_DI_OP(left, !=, right));
        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_ID_OP(left, !=, right));
        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(BINARY_D_OP(left, !=, right));
        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject * left, RuntimeObject * right, stack);
        stack.push_back(CreateBool(GetBool(left) != GetBool(right)));
        break;
    }
    case Opcode::BANG:
    {
        RuntimeObject *right = stack.back;
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
    {
        char *str = RTOToString(stack[stack.count - arity + i]);
        std::cout << str;
        free(str);
    }

    std::cout << std::endl;
    stack.pop_N((size_t)arity);
}

void VM::NativeToString(int)
{
    char *chrs = RTOToString(stack.back);
    String rtstr = {strlen(chrs), chrs};
    stack.pop_back();
    stack.push_back(CreateString(rtstr));
}