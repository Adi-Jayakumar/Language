#include "vm.h"

VM::VM(std::vector<Chunk> &_functions)
{
    functions = _functions;

    cs = new CallFrame[UINT8_MAX];
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
    Array s = stack;
    std::cout << "index |||| value" << std::endl;
    for (size_t i = s.count - 1; (int)i >= 0; i--)
    {
        std::cout << i << " " << s[i] << std::endl;
    }
}

void VM::Jump(size_t jump)
{
    ip += jump;
}

void VM::ExecuteCurrentChunk()
{
    // while (cs.Size() >= 1)
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
    // pushes the constant at o.op1's location + constOffset onto the stack
    case Opcode::GET_C:
    {
        CompileConst c = functions[curChunk].constants[o.op1];
        stack.push_back(c);
        break;
    }
    // updates the value on the stack at the relative index given by
    // the operand to the value currently at the top of the stack
    case Opcode::VAR_A:
    {
        CompileConst value = *stack.back;
        stack[o.op1 + curCF->valStackMin] = value;
        break;
    }
    // returns the value of the variable at o.op1's location + varOffset
    case Opcode::GET_V:
    {
        CompileConst v = stack[o.op1 + curCF->valStackMin];
        if (curChunk == 0)
            std::cout << "Var val: " << v << std::endl;
        stack.push_back(v);
        break;
    }
    // adds the operand to the ip if the value on the top of the stack is not truthy
    case Opcode::JUMP_IF_FALSE:
    {
        if (!IsTruthy(*stack.back))
            ip += o.op1;
        stack.pop_back();
        break;
    }
    // adds the operand to the ip
    case Opcode::JUMP:
    {
        ip += o.op1;
        break;
    }
    // op1 is the index of the function, op2 is the arity of the function called
    case Opcode::CALL_F:
    {
        curCF++;

        if (curCF == &cs[UINT8_MAX - 1])
        {
            std::cout << "CallStack overflow." << std::endl;
            exit(3);
        }
        
        *curCF = {ip + 1, curChunk, stack.count - functions[curChunk].arity};

        curChunk = o.op1;
        // curCF = cs.Top();
        ip = -1;
        break;
    }
    // returns from the current function and cleans up the current function's constants
    // operand is 1 if there is nothing to return
    case Opcode::RETURN:
    {
        CallFrame *returnCF = curCF;
        curCF--;

        ip = returnCF->retIndex - 1;
        curChunk = returnCF->retChunk;

        size_t stackDiff = stack.count - returnCF->valStackMin;
        CompileConst retVal;

        if (o.op1 == 0)
            retVal = *stack.back;

        // cleaning up the function's constants
        stack.count -= stackDiff;
        stack.back = &stack.data[stack.count - 1];

        if (o.op1 == 0)
            stack.push_back(retVal);

        break;
    }
    // adds the last 2 things on the stack
    case Opcode::ADD:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left + right);
        break;
    }
    // subtracts the last 2 things on the stack
    case Opcode::SUB:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left - right);
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::MUL:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left * right);
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::DIV:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left / right);
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::GT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left > right);
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::LT:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left < right);
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::GEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left >= right);
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::LEQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left <= right);
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::EQ_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left == right);
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::BANG_EQ:
    {
        CompileConst right = *stack.back;
        stack.pop_back();
        CompileConst left = *stack.back;
        stack.pop_back();
        stack.push_back(left != right);
        break;
    }
    // Does nothing
    case Opcode::NONE:
    {
        break;
    }
    }
}