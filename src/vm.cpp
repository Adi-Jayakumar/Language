#include "vm.h"

VM::VM(std::vector<Chunk> &_functions)
{
    stack = Stack(512);
    functions = _functions;

    cs = CallStack();
    cs.Push({0, 0, 0});

    curCF = cs.Top();

    curChunk = 0;
    ip = 0;
}

void VM::SetChunk(size_t n)
{
    curChunk = n;
}

void VM::PrintStack()
{
    Stack s = stack;
    std::cout << "index |||| value" << std::endl;
    for (size_t i = s.s.size() - 1; (int)i >= 0; i--)
    {
        std::cout << i << " " << s.s[i] << std::endl;
    }
}

void VM::Jump(size_t jump)
{
    ip += jump;
}

void VM::ExecuteCurrentChunk()
{
    while (cs.Size() >= 1)
    {
        while (ip != functions[curChunk].code.size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        if (cs.Size() != 1)
        {
            std::cout << "Finished executing fucntion " << curChunk << std::endl;
            // CallFrame with the details of where to return to
            CallFrame returnCF = cs.Top();
            cs.Pop();
            ip = returnCF.retIndex;
            curChunk = returnCF.retChunk;
            curCF = cs.Top();
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
        stack.Pop();
        break;
    }
    // returns the constant at o.op1's location + constOffset
    case Opcode::GET_C:
    {
        CompileConst c = functions[curChunk].constants[o.op1];
        stack.Push(c);
        break;
    }
    case Opcode::VAR_A:
    {
        CompileConst value = stack.Top();
        stack[o.op1 + curCF.valStackMin] = value;
        break;
    }
    // returns the value of the variable at o.op1's location + varOffset
    case Opcode::GET_V:
    {
        CompileConst v = stack[o.op1 + curCF.valStackMin];
        if (curChunk == 0)
            std::cout << "Var val: " << v << std::endl;
        stack.Push(v);
        break;
    }
    // adds the operand to the ip if the value on the top of the stack is not truthy
    case Opcode::JUMP_IF_FALSE:
    {
        if (!IsTruthy(stack.Top()))
            ip += o.op1;
        stack.Pop();
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

        // std::cout << "Calling function: " << o.op1 << std::endl;

        cs.Push({ip + 1, curChunk, stack.Size() - o.op2});

        curChunk = o.op1;
        curCF = cs.Top();
        ip = -1;
        break;
    }
    case Opcode::RETURN:
    {
        // std::cout << "Returning from function: " << curChunk << std::endl;
        CallFrame returnCF = cs.Top();
        cs.Pop();
        ip = returnCF.retIndex - 1;
        curChunk = returnCF.retChunk;
        curCF = cs.Top();

        size_t stackDiff = stack.Size() - returnCF.valStackMin;
        CompileConst retVal;

        if (o.op1 == 0)
            retVal = stack.Top();

        // cleaning up the function's constants
        stack.s.resize(stack.s.size() - stackDiff);

        if (o.op1 == 0)
            stack.Push(retVal);

        break;
    }
    // adds the last 2 things on the stack
    case Opcode::ADD:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left + right);
        break;
    }
    // subtracts the last 2 things on the stack
    case Opcode::SUB:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left - right);
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::MUL:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left * right);
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::DIV:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left / right);
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::GT:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left > right);
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::LT:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left < right);
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::GEQ:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left >= right);
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::LEQ:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left <= right);
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::EQ_EQ:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left == right);
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::BANG_EQ:
    {
        CompileConst right = stack.Top();
        stack.Pop();
        CompileConst left = stack.Top();
        stack.Pop();
        stack.Push(left != right);
        break;
    }
    // Does nothing
    case Opcode::NONE:
    {
        break;
    }
    }
}