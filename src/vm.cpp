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

    RuntimeObject *cc = (RuntimeObject *)malloc(stack.count * sizeof(RuntimeObject));
    memcpy(cc, stack.data, stack.count * sizeof(RuntimeObject));

    std::cout << "index\t|\tvalue" << std::endl;

    for (size_t i = stack.count - 1; (int)i >= 0; i--)
    {
        std::cout << i << "\t|\t" << cc[i] << std::endl;
    }
    free(cc);
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

    if (stack.count != 0)
        for (size_t i = 0; i < stack.count; i++)
        {
            if (stack.data[i].t.type == 4)
                free(stack.data[i].as.arr.data);
        }

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
        stack.back = &stack.data[stack.count];
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
    // pops the top value off the stack
    case Opcode::POP:
    {
        // if (stack.back->t.type == 4)
        //     free(stack.back->as.arr.data);
        stack.pop_back();

        break;
    }
    // pushes the constant at o.op's location + constOffset onto the stack
    case Opcode::GET_C:
    {
        RuntimeObject c = functions[curChunk].constants[o.op];
        stack.push_back(c);
        break;
    }
    // updates the value on the stack at the relative index given by
    // the operand to the value currently at the top of the stack
    case Opcode::VAR_A:
    {
        RuntimeObject value = *stack.back;
        stack[o.op + curCF->valStackMin] = value;
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
    // returns the value of the variable at o.op's location + varOffset
    case Opcode::GET_V:
    {
        RuntimeObject v = stack[o.op + curCF->valStackMin];
        stack.push_back(v);
        break;
    }
    // pushes the global variable at o.op's location
    case Opcode::GET_V_GLOBAL:
    {
        RuntimeObject v = globals[o.op];
        stack.push_back(v);
        break;
    }
    // populates the array which is at the top of the stack, the values are
    // in the stack slots below the array
    case Opcode::ARR_D:
    {
        RuntimeObject arrAsCC = stack.back[-o.op];

        RTArray arr = arrAsCC.as.arr;
        RuntimeObject *arrStart = stack.back - arr.size + 1;

        for (size_t i = 0; i < arr.size; i++)
            arr.data[i] = arrStart[i];

        stack.back = arrStart - 1;
        stack.count -= arr.size;

        break;
    }
    // fetches the value at the index's location (top of the stack) from the array
    // on the slot one below the index
    case Opcode::ARR_INDEX:
    {
        RuntimeObject index = *stack.back;
        stack.pop_back();
        RuntimeObject arrayAsCC = *stack.back;
        if (arrayAsCC.t.type == 0)
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        RTArray arr = arrayAsCC.as.arr;

        if (index.as.i >= (int)arr.size || index.as.i < 0)
            RuntimeError("Array index " + std::to_string(index.as.i) + " out of bounds for array of size " + std::to_string(arr.size));

        stack.push_back(arr.data[index.as.i]);

        break;
    }
    // Allocates an array whose size is the top value on the stack and pushes
    // the allocated array onto the stack
    case Opcode::ARR_ALLOC:
    {
        RuntimeObject size = *stack.back;
        stack.pop_back();

        if (size.as.i < 0)
            RuntimeError("Cannot have a negative size for a dynamically allocated array size");

        RTArray arr;
        arr.data = Allocate(size.as.i);
        arr.size = size.as.i;
        stack.push_back(RuntimeObject(RuntimeObject(arr)));
        break;
    }
    // sets the index
    case Opcode::ARR_SET:
    {
        RuntimeObject arrayAsCC = *stack.back;
        if (arrayAsCC.t.type == 0)
            RuntimeError("Cannot set an index of an uninitialised array");
        RTArray arr = arrayAsCC.as.arr;
        stack.pop_back();

        RuntimeObject index = *stack.back;
        stack.pop_back();

        RuntimeObject value = *stack.back;
        stack.pop_back();

        if (index.as.i > (int)arr.size)
            RuntimeError("Array index " + std::to_string(index.as.i) + " out of bounds for array of size " + std::to_string(arr.size));

        arr.data[index.as.i] = value;

        stack.push_back(value);
        break;
    }
    // adds the operand to the ip if the value on the top of the stack is not truthy
    case Opcode::JUMP_IF_FALSE:
    {
        if (!IsTruthy(*stack.back))
            ip += o.op;
        stack.pop_back();
        break;
    }
    // adds the operand to the ip
    case Opcode::JUMP:
    {
        ip += o.op;
        break;
    }
    // sets the ip to the operand used in loops
    case Opcode::LOOP:
    {
        ip = o.op;
        break;
    }
    // op is the index of the function, op2 is the arity of the function called
    case Opcode::CALL_F:
    {
        curCF++;

        if (curCF == &cs[STACK_MAX - 1])
        {
            std::cout << (curCF - &cs[0]) << std::endl;
            std::cout << "CallStack overflow." << std::endl;
            exit(3);
        }

        *curCF = {ip, curChunk, stack.count - functions[o.op].arity};

        curChunk = o.op;
        ip = -1;
        break;
    }
    // returns from the current function and cleans up the current function's constants
    // operand is 1 if there is nothing to return
    case Opcode::RETURN:
    {
        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        curChunk = returnCF->retChunk;

        size_t stackDiff = stack.count - returnCF->valStackMin;
        RuntimeObject retVal;

        if (o.op == 0)
            retVal = *stack.back;

        // cleaning up the function's constants
        stack.count -= stackDiff;
        stack.back = &stack.data[stack.count];

        if (o.op == 0)
            stack.push_back(retVal);

        break;
    }
    case Opcode::NATIVE_CALL:
    {
        RuntimeObject arityAsCC = *stack.back;
        stack.pop_back();
        RuntimeObject *args = stack.back - arityAsCC.as.i + 1;

        switch (o.op)
        {
        case 0:
        {
            NativePrint(args, arityAsCC.as.i);
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
        stack.push_back(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, +, right));

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

        stack.push_back(RuntimeObject(concatStr));
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

            stack.push_back(BINARY_I_OP(left, -, right));
        }
        else
            stack.push_back(UNARY_I_OP(-, right));
        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, -, right));
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

            stack.push_back(BINARY_D_OP(left, -, right));
        }
        else
            stack.push_back(UNARY_D_OP(-, right));

        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, *, right));

        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, *, right));

        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, *, right));

        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, *, right));

        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, /, right));

        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, /, right));

        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, /, right));

        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, /, right));

        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, >, right));

        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, >, right));

        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, >, right));

        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, >, right));

        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, <, right));

        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, <, right));

        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, <, right));

        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, <, right));

        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, >=, right));

        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, >=, right));

        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, >=, right));

        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, >=, right));

        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, <=, right));

        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, <=, right));

        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, <=, right));

        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, <=, right));

        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, ==, right));

        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, ==, right));

        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, ==, right));

        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, ==, right));

        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(RuntimeObject(left.as.b == right.as.b));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_I_OP(left, !=, right));

        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_DI_OP(left, !=, right));

        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_ID_OP(left, !=, right));

        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(BINARY_D_OP(left, !=, right));

        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(RuntimeObject left, RuntimeObject right, stack);
        stack.push_back(RuntimeObject(left.as.b != right.as.b));
        break;
    }
    case Opcode::BANG:
    {
        RuntimeObject right = *stack.back;
        stack.pop_back();

        stack.push_back(UNARY_B_OP(!, right));
    }
    // Does nothing
    case Opcode::NONE:
    {
        break;
    }
    }
}

void VM::NativePrint(RuntimeObject *args, int arity)
{
    for (size_t i = 0; i < (size_t)arity; i++)
        std::cout << args[i];

    std::cout << std::endl;

    for (size_t j = 0; j < (size_t)arity; j++)
        stack.pop_back();
}