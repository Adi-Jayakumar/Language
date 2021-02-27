#include "vm.h"

VM::VM(std::vector<Chunk> &_functions, size_t mainIndex)
{
    functions = _functions;

    cs = new CallFrame[STACK_MAX];
    curCF = cs;
    curChunk = 0;
    *curCF = {0, mainIndex, 0};
    curCF++;
    *curCF = {0, mainIndex, 0};

    ip = 0;
}

VM::~VM()
{
    delete[] cs;
}

void VM::SetChunk(size_t n)
{
    curChunk = n;
}

void VM::PrintStack()
{
    std::cout << "index\t|\tvalue" << std::endl;

    for (size_t i = stack.count - 1; (int)i >= 0; i--)
    {
        std::cout << i << "\t|\t" << *stack[i] << std::endl;
    }
}

RuntimeObject *VM::Allocate(size_t size)
{
    return (RuntimeObject *)malloc(size * sizeof(RuntimeObject));
}

char *VM::StringAllocate(size_t size)
{
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
    if (curChunk == SIZE_MAX)
        return;
    while (curCF != cs - 1)
    {
        while (ip < functions[curChunk].code.size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        if (curChunk != 0)
            ip++;

        curChunk = returnCF->retChunk;
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
    Op o = functions[curChunk].code[ip];
    switch (o.code)
    {
    case Opcode::POP:
    {
        stack.pop_back();
        break;
    }
    case Opcode::GET_C:
    {
        stack.push_back(&functions[curChunk].constants[o.op]);
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

        if (array->t.type == 0)
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        if (index->as.i >= (int)array->as.arr.size || index->as.i < 0)
            RuntimeError("Array index " + std::to_string(index->as.i) + " out of bounds for array of size " + std::to_string(array->as.arr.size));

        stack.push_back(&array->as.arr.data[index->as.i]);

        break;
    }
    case Opcode::ARR_SET:
    {
        RuntimeObject *array = stack.back;
        stack.pop_back();

        size_t arraySize = array->as.arr.size;

        RuntimeObject index = *stack.back;
        stack.pop_back();
        int i = index.as.i;

        RuntimeObject value = *stack.back;
        stack.pop_back();
        if (i >= (int)arraySize || i < 0)
            RuntimeError("Array index " + std::to_string(i) + " out of bounds for array of size " + std::to_string(arraySize));

        array->as.arr.data[i] = value;
        stack.push_back_copy(value);
        break;
    }
    case Opcode::ARR_ALLOC:
    {
        RuntimeObject size = *stack.back;
        stack.pop_back();
        int arraySize = size.as.arr.size;

        if (arraySize <= 0)
            RuntimeError("Dynamically allocated array must be declared with a size greater than 0");

        stack.push_back_copy(RuntimeObject({true, 1}, static_cast<size_t>(arraySize)));
        break;
    }
    case Opcode::JUMP_IF_FALSE:
    {
        if (!stack.back->as.b)
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
        {
            std::cout << (curCF - &cs[0]) << std::endl;
            RuntimeError("CallStack overflow.");
        }

        *curCF = {ip, curChunk, stack.count - functions[o.op].arity};

        curChunk = o.op;
        ip = -1;
        break;
    }
    case Opcode::RETURN:
    {
        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        curChunk = returnCF->retChunk;

        size_t stackDiff = stack.count - returnCF->valStackMin;
        RuntimeObject *retVal;

        if (o.op == 0)
            retVal = stack.back;

        // cleaning up the function's constants
        // stack.count -= stackDiff;
        // stack.back = stack.data[stack.count];
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
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, +, right));

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

        stack.push_back_copy(RuntimeObject(concatStr));
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

            stack.push_back_copy(BINARY_I_OP(left, -, right));
        }
        else
            stack.push_back_copy(UNARY_I_OP(-, right));
        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, -, right));
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

            stack.push_back_copy(BINARY_D_OP(left, -, right));
        }
        else
            stack.push_back_copy(UNARY_D_OP(-, right));

        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, *, right));

        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, *, right));

        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, *, right));

        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, *, right));

        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, /, right));

        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, /, right));

        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, /, right));

        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, /, right));

        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, >, right));

        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, >, right));

        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, >, right));

        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, >, right));

        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, <, right));

        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, <, right));

        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, <, right));

        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, <, right));

        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, >=, right));

        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, >=, right));

        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, >=, right));

        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, >=, right));

        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, <=, right));

        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, <=, right));

        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, <=, right));

        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, <=, right));

        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, ==, right));

        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, ==, right));

        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, ==, right));

        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, ==, right));

        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(RuntimeObject(left.as.b == right.as.b));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_I_OP(left, !=, right));

        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_DI_OP(left, !=, right));

        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_ID_OP(left, !=, right));

        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(BINARY_D_OP(left, !=, right));

        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back_copy(RuntimeObject(left.as.b != right.as.b));
        break;
    }
    case Opcode::BANG:
    {
        RuntimeObject right = *stack.back;
        stack.pop_back();

        stack.push_back_copy(UNARY_B_OP(!, right));
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