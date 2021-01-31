#include "vm.h"

VM::VM(std::vector<Chunk> &_functions)
{
    functions = _functions;

    cs = new CallFrame[STACK_MAX];
    curCF = cs;
    *curCF = {0, 0, 0};

    curChunk = 0;
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

void VM::Jump(size_t jump)
{
    ip += jump;
}

void VM::ExecuteCurrentChunk()
{
    while (true)
    {
        while (ip != functions[curChunk].code.size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        if ((curCF - cs) != 0)
        {

            // CallFrame with the details of where to return to
            CallFrame *returnCF = curCF;
            curCF--;
            ip = returnCF->retIndex;
            curChunk = returnCF->retChunk;
        }
        else
            break;
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

void VM::ExecuteInstruction()
{
    Op o = functions[curChunk].code[ip];
    switch (o.code)
    {
    // pops the top value off the stack
    case Opcode::POP:
    {
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
    // returns the value of the variable at o.op's location + varOffset
    case Opcode::GET_V:
    {
        CompileConst v = stack[o.op + curCF->valStackMin];
        if (curChunk == 0)
            std::cout << "chunk: " << curChunk << " val: " << v << std::endl;
        stack.push_back(v);
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
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, +, right));

        break;
    }
        // SUBTRACTIONS: subtracts the last 2 things on the stack
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
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

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
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, *, right));

        break;
    }
    case Opcode::DI_MUL:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, *, right));

        break;
    }
    case Opcode::ID_MUL:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, *, right));

        break;
    }
    case Opcode::D_MUL:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, *, right));

        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, /, right));

        break;
    }
    case Opcode::DI_DIV:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, /, right));

        break;
    }
    case Opcode::ID_DIV:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, /, right));

        break;
    }
    case Opcode::D_DIV:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, /, right));

        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, >, right));

        break;
    }
    case Opcode::DI_GT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, >, right));

        break;
    }
    case Opcode::ID_GT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, >, right));

        break;
    }
    case Opcode::D_GT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, >, right));

        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, <, right));

        break;
    }
    case Opcode::DI_LT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, <, right));

        break;
    }
    case Opcode::ID_LT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, <, right));

        break;
    }
    case Opcode::D_LT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, <, right));

        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, >=, right));

        break;
    }
    case Opcode::DI_GEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, >=, right));

        break;
    }
    case Opcode::ID_GEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, >=, right));

        break;
    }
    case Opcode::D_GEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, >=, right));

        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, <=, right));

        break;
    }
    case Opcode::DI_LEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, <=, right));

        break;
    }
    case Opcode::ID_LEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, <=, right));

        break;
    }
    case Opcode::D_LEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, <=, right));

        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, ==, right));

        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, ==, right));

        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, ==, right));

        break;
    }
    case Opcode::D_EQ_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, ==, right));

        break;
    }
    case Opcode::B_EQ_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(CompileConst(left.as.b == right.as.b));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_I_OP(left, !=, right));

        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_DI_OP(left, !=, right));

        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_ID_OP(left, !=, right));

        break;
    }
    case Opcode::D_BANG_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

        stack.push_back(BINARY_D_OP(left, !=, right));

        break;
    }
    case Opcode::B_BANG_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();

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
