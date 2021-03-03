#include "vm.h"

VM::VM(std::vector<RuntimeFunction> &_functions, size_t mainIndex)
{
    functions = _functions;

    cs = new CallFrame[STACK_MAX];
    curCF = cs;
    curFunc = 0;
    *curCF = {0, mainIndex, 0};
    curCF++;
    *curCF = {0, mainIndex, 0};

    ip = 0;
}

VM::~VM()
{
    delete[] cs;
#ifndef GC_STRESS_TEST
    for (RuntimeFunction &rtf : functions)
    {
        for (RuntimeObject &rto : rtf.values)
        {
            if (rto.state != GCSate::FREED)
                GC::FreeObject(&rto);
        }
    }
    for (size_t i = 0; i < RTAllocValues.count; i++)
        GC::DestroyObject(RTAllocValues[i]);

#endif
}

void VM::PrintStack()
{
    std::cout << "index\t|\tvalue" << std::endl;
    for (size_t i = stack.count - 1; (int)i >= 0; i--)
        std::cout << i << "\t|\t" << *stack[i] << std::endl;
}

RuntimeObject *VM::Allocate(size_t size)
{
    // GC::GarbageCollect(this);
    return (RuntimeObject *)malloc(size * sizeof(RuntimeObject));
}

char *VM::StringAllocate(size_t size)
{
    // GC::GarbageCollect(this);
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
        }

        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        if (curFunc != 0)
            ip++;

        curFunc = returnCF->retChunk;
        size_t stackDiff = stack.count - returnCF->valStackMin;

        // cleaning up the function's constants
        stack.count -= stackDiff;
        stack.back = stack[stack.count];
    }
}

#define BINARY_I_OP(l, op, r) \
    RuntimeObject(l.as.i op r.as.i)

#define BINARY_DI_OP(l, op, r) \
    RuntimeObject(l.as.d op r.as.i)

#define BINARY_ID_OP(l, op, r) \
    RuntimeObject(l.as.i op r.as.d)

#define BINARY_D_OP(l, op, r) \
    RuntimeObject(l.as.d op r.as.d)

#define UNARY_I_OP(op, r) \
    RuntimeObject(op r.as.i)

#define UNARY_D_OP(op, r) \
    RuntimeObject(op r.as.d)

#define UNARY_B_OP(op, r) \
    RuntimeObject(op r.as.b)

#define TAKE_LEFT_RIGHT(left, right, stack) \
    right = *stack.back;                    \
    stack.pop_back();                       \
    left = *stack.back;                     \
    stack.pop_back()

void VM::ExecuteInstruction()
{
    Op o = functions[curFunc].code[ip];
    switch (o.code)
    {
    case Opcode::POP:
    {
        stack.back->state = GCSate::UNMARKED;
        stack.pop_back();
        break;
    }
    case Opcode::GET_C:
    {
        stack.push_back(&functions[curFunc].values[o.op]);
        break;
    }
    case Opcode::VAR_A:
    {
        *stack[o.op + curCF->valStackMin] = *stack.back;
        break;
    }
    case Opcode::VAR_A_GLOBAL:
    {
        globals[o.op] = *stack.back;
        break;
    }
    case Opcode::VAR_D_GLOBAL:
    {
        globals.push_back(*stack.back);
        break;
    }
    case Opcode::GET_V:
    {
        stack.push_back(stack[o.op + curCF->valStackMin]);
        break;
    }
    case Opcode::GET_V_GLOBAL:
    {
        stack.push_back(&globals[o.op]);
        break;
    }
    case Opcode::ARR_D:
    {
        RuntimeObject *emptyArr = stack[stack.count - o.op - 1];

        for (size_t i = 0; i < o.op; i++)
            emptyArr->as.arr.data[i] = *stack[stack.count - o.op + i];

        stack.pop_N(o.op);
        break;
    }
    case Opcode::ARR_INDEX:
    {
        RuntimeObject *index = stack.back;
        stack.pop_back();
        RuntimeObject *array = stack.back;

        if (array->t == RuntimeType::NULL_T)
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        int size = (array->t == RuntimeType::ARRAY) ? (int)array->as.arr.size : (int)array->as.str.len;

        if (index->as.i >= (int)size || index->as.i < 0)
            RuntimeError("Array index " + std::to_string(index->as.i) + " out of bounds for array of size " + std::to_string(array->as.arr.size));

        if (array->t == RuntimeType::ARRAY)
            stack.push_back(&array->as.arr.data[index->as.i]);
        else
        {
            RuntimeObject *copy = Allocate(1);
            stack.push_back_copy(copy, RuntimeObject(array->as.str.data[index->as.i]));
            RTAllocValues.push_back(copy);
        }
        break;
    }
    case Opcode::ARR_SET:
    {
        RuntimeObject index = *stack.back;
        stack.pop_back();
        int i = index.as.i;

        RuntimeObject *array = stack.back;
        stack.pop_back();

        size_t arraySize = array->as.arr.size;

        RuntimeObject value = *stack.back;
        stack.pop_back();
        if (i >= (int)arraySize || i < 0)
            RuntimeError("Array index " + std::to_string(i) + " out of bounds for array of size " + std::to_string(arraySize));

        array->as.arr.data[i] = value;
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, value);
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ARR_ALLOC:
    {
        RuntimeObject size = *stack.back;
        stack.pop_back();
        int arraySize = size.as.arr.size;

        if (arraySize <= 0)
            RuntimeError("Dynamically allocated array must be declared with a size greater than 0");

        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, RuntimeObject(RuntimeType::ARRAY, static_cast<size_t>(arraySize)));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::JUMP_IF_FALSE:
    {
        if (!IsTruthy(*stack.back))
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
            stack.push_back(retVal);
        break;
    }
    case Opcode::NATIVE_CALL:
    {
        RuntimeObject arityAsCC = *stack.back;
        stack.pop_back();

        switch (o.op)
        {
        case 0:
        {
            NativePrint(arityAsCC.as.i);
            break;
        }
        case 1:
        {
            NativeToString(arityAsCC.as.i);
        }
        default:
            break;
        }
        break;
    }
    case Opcode::STRUCT_MEMBER:
    {
        RuntimeObject *strct = stack.back;
        stack.push_back(&strct->as.strct[o.op]);
        break;
    }
    case Opcode::STRUCT_D:
    {
        RuntimeObject *emptyStruct = stack[stack.count - o.op - 1];

        for (size_t i = 0; i < o.op; i++)
            emptyStruct->as.strct[i] = *stack[stack.count - o.op + i];

        stack.pop_N(o.op);
        break;
    }
    case Opcode::STRUCT_MEMBER_SET:
    {
        RuntimeObject *strct = stack.back;
        stack.pop_back();

        RuntimeObject val = *stack.back;

        strct->as.strct[o.op] = val;
        break;
    }
    // ADDITIONS: adds the last 2 things on the stack
    case Opcode::I_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, +, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, +, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, +, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, +, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::S_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RTString lStr = left.as.str;
        RTString rStr = right.as.str;

        size_t newStrSize = lStr.len + rStr.len;

        char *concat = StringAllocate(newStrSize + 1);
        strcpy(concat, lStr.data);

        char *next = concat + lStr.len;
        strcpy(next, rStr.data);

        RTString concatStr = {newStrSize, concat};

        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, RuntimeObject(concatStr));
        RTAllocValues.push_back(copy);
        break;
    }
    // SUBTRACTIONS: subtracts the last 2 things on the stack
    // if o.op is 1 then is a unary negation (only the case
    // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        RuntimeObject right = *stack.back;
        stack.pop_back();
        if (o.op == 0)
        {
            RuntimeObject left = *stack.back;
            stack.pop_back();

            RuntimeObject *copy = Allocate(1);
            stack.push_back_copy(copy, BINARY_I_OP(left, -, right));
            RTAllocValues.push_back(copy);
        }
        else
        {
            RuntimeObject *copy = Allocate(1);
            stack.push_back_copy(copy, UNARY_I_OP(-, right));
            RTAllocValues.push_back(copy);
        }
        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, -, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, -, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_SUB:
    {
        RuntimeObject right = *stack.back;
        stack.pop_back();

        if (o.op == 0)
        {
            RuntimeObject left = *stack.back;
            stack.pop_back();

            RuntimeObject *copy = Allocate(1);
            stack.push_back_copy(copy, BINARY_D_OP(left, -, right));
            RTAllocValues.push_back(copy);
        }
        else
        {
            RuntimeObject *copy = Allocate(1);
            stack.push_back_copy(copy, UNARY_D_OP(-, right));
            RTAllocValues.push_back(copy);
        }
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, *, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, *, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, *, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, *, right));
        RTAllocValues.push_back(copy);
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, /, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, /, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, /, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, /, right));
        RTAllocValues.push_back(copy);
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, >, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, >, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, >, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, >, right));
        RTAllocValues.push_back(copy);
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, <, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, <, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, <, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, <, right));
        RTAllocValues.push_back(copy);
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, >=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, >=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, >=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, >=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, <=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, <=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, <=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, <=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, ==, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, ==, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, ==, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, ==, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, RuntimeObject(left.as.b == right.as.b));
        RTAllocValues.push_back(copy);
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_I_OP(left, !=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_DI_OP(left, !=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_ID_OP(left, !=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, BINARY_D_OP(left, !=, right));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, RuntimeObject(left.as.b != right.as.b));
        RTAllocValues.push_back(copy);
        break;
    }
    case Opcode::BANG:
    {
        RuntimeObject right = *stack.back;
        stack.pop_back();

        RuntimeObject *copy = Allocate(1);
        stack.push_back_copy(copy, UNARY_B_OP(!, right));
        RTAllocValues.push_back(copy);
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
        std::cout << *stack[stack.count - arity + i];

    std::cout << std::endl;
    stack.pop_N((size_t)arity);
}

void VM::NativeToString(int)
{
    RuntimeObject rtstr = RuntimeObject(ToString(*stack.back).data());
    stack.pop_back();
    RuntimeObject *copy = Allocate(1);
    stack.push_back_copy(copy, rtstr);
    RTAllocValues.push_back(copy);
}