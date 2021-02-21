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

    CompileConst *cc = (CompileConst *)malloc(stack.count * sizeof(CompileConst));
    memcpy(cc, stack.data, stack.count * sizeof(CompileConst));

    std::cout << "index\t|\tvalue" << std::endl;

    for (size_t i = stack.count - 1; (int)i >= 0; i--)
    {
        std::cout << i << "\t|\t" << cc[i] << std::endl;
    }
    free(cc);
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

void VM::ExecuteCurrentChunk()
{
    if (curChunk == SIZE_MAX)
        return;
    while (curCF != cs - 1)
    {
        while (ip < functions[curChunk].code.size())
        {
            // std::cout << "cur ins " << ToString(functions[curChunk].code[ip].code) << " " << +functions[curChunk].code[ip].op << std::endl;
            ExecuteInstruction();
            Jump(1);
        }

        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex;
        if (curChunk != 0)
            ip++;

        // std::cout << "returnCF retChunk: " << returnCF->retChunk << std::endl;
        // std::cout << "returnCF retIndex: " << returnCF->retIndex << std::endl;
        // std::cout << "returnCF valstackmin: " << returnCF->valStackMin << std::endl;

        // std::cout << "ip: " << ip << std::endl;
        // std::cout << "returning from: " << curChunk << std::endl;
        curChunk = returnCF->retChunk;
        // std::cout << "returning to: " << curChunk << std::endl;
        // std::cout << "cs diff: " << curCF - cs << std::endl;
        size_t stackDiff = stack.count - returnCF->valStackMin;

        // cleaning up the function's constants
        stack.count -= stackDiff;
        stack.back = &stack.data[stack.count];
    }
}

#define BINARY_I_OP(l, op, r) \
    CompileConst(l.as.i op r.as.i)

#define BINARY_DI_OP(l, op, r) \
    CompileConst(l.as.d op r.as.i)

#define BINARY_ID_OP(l, op, r) \
    CompileConst(l.as.i op r.as.d)

#define BINARY_D_OP(l, op, r) \
    CompileConst(l.as.d op r.as.d)

#define UNARY_I_OP(op, r) \
    CompileConst(op r.as.i)

#define UNARY_D_OP(op, r) \
    CompileConst(op r.as.d)

#define UNARY_B_OP(op, r) \
    CompileConst(op r.as.b)

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
        CompileConst c = functions[curChunk].constants[o.op];
        stack.push_back(c);
        break;
    }
    // updates the value on the stack at the relative index given by
    // the operand to the value currently at the top of the stack
    case Opcode::VAR_A:
    {
        CompileConst value = *stack.back;
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
        CompileConst v = stack[o.op + curCF->valStackMin];
        // if (curChunk != 3)
        if (v.t.type != 4)
            std::cout << "chunk: " << curChunk << " val: " << v << std::endl;
        stack.push_back(v);
        break;
    }
    case Opcode::GET_V_GLOBAL:
    {
        CompileConst v = globals[o.op];
        if (v.t.type != 4)
            std::cout << "chunk: " << curChunk << " val: " << v << std::endl;
        stack.push_back(v);
        break;
    }
    // populates the array which is at the top of the stack, the values are
    // in the stack slots below the array
    case Opcode::ARR_D:
    {
        CompileConst arrAsCC = *stack.back;

        CCArray arr = arrAsCC.as.arr;
        CompileConst *arrStart = stack.back - arr.size;

        for (size_t i = 0; i < arr.size; i++)
            arr.data[i] = arrStart[i];

        stack.back = arrStart;
        stack.count -= arr.size + 1;

        stack.push_back(arrAsCC);

        break;
    }
    // fetches the value at the index's location (top of the stack) from the array
    // on the slot one below the index
    case Opcode::ARR_INDEX:
    {
        CompileConst index = *stack.back;
        stack.pop_back();
        CompileConst arrayAsCC = *stack.back;
        stack.pop_back();

        CCArray arr = arrayAsCC.as.arr;

        if (index.as.i > (int)arr.size)
            RuntimeError("Array index " + std::to_string(index.as.i) + " out of bounds for array of size " + std::to_string(arr.size));

        stack.push_back(arr.data[index.as.i]);

        break;
    }
    // sets the index
    case Opcode::ARR_SET:
    {
        CompileConst arrayAsCC = *stack.back;
        CCArray arr = arrayAsCC.as.arr;
        stack.pop_back();

        CompileConst index = *stack.back;
        stack.pop_back();

        CompileConst value = *stack.back;
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
        CompileConst retVal;

        if (o.op == 0)
            retVal = *stack.back;

        // cleaning up the function's constants
        stack.count -= stackDiff;
        stack.back = &stack.data[stack.count];

        if (o.op == 0)
            stack.push_back(retVal);

        break;
    }
        // ADDITIONS: adds the last 2 things on the stack
    case Opcode::I_ADD:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, +, right));

        break;
    }
        // SUBTRACTIONS: subtracts the last 2 things on the stack
        // if o.op is 1 then is a unary negation (only the case
        // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        if (o.op == 0)
        {
            CompileConst left = *stack.back;
            stack.pop_back();

            stack.push_back(BINARY_I_OP(left, -, right));
        }
        else
        {
            stack.push_back(UNARY_I_OP(-, right));
        }

        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, -, right));
        break;
    }
    case Opcode::D_SUB:
    {
        CompileConst right = *stack.back;
        stack.pop_back();

        if (o.op == 0)
        {
            CompileConst left = *stack.back;
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
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, *, right));

        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, *, right));

        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, *, right));

        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, *, right));

        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, /, right));

        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, /, right));

        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, /, right));

        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, /, right));

        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, >, right));

        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, >, right));

        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, >, right));

        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, >, right));

        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, <, right));

        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, <, right));

        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, <, right));

        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, <, right));

        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, >=, right));

        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, >=, right));

        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, >=, right));

        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, >=, right));

        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, <=, right));

        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, <=, right));

        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, <=, right));

        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, <=, right));

        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, ==, right));

        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, ==, right));

        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, ==, right));

        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, ==, right));

        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(CompileConst(left.as.b == right.as.b));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_I_OP(left, !=, right));

        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_DI_OP(left, !=, right));

        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_ID_OP(left, !=, right));

        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(BINARY_D_OP(left, !=, right));

        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(CompileConst left, CompileConst right, stack);
        stack.push_back(CompileConst(left.as.b != right.as.b));
        break;
    }
    case Opcode::BANG:
    {
        CompileConst right = *stack.back;
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
