#include "vm.h"

VM::VM(std::vector<Chunk> &_functions)
{
    stack = Stack();
    functions = _functions;

    cs = CallStack();
    cs.Push({0, 0, 0, 0});

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
    while (!s.Empty())
    {
        std::cout << s.Top() << std::endl;
        s.Pop();
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
            // std::cout << ToString(functions[curChunk].code[ip].code) << " " << functions[curChunk].code[ip].op1 << " " << functions[curChunk].code[ip].op2 << std::endl;
            ExecuteInstruction();
            Jump(1);
        }

        if (cs.Size() != 1)
        {
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
    // Op o = *ip;
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
    // pops the value currently on the top of the stack and assigns it to a CompileVar at o.op1's location
    case Opcode::VAR_D:
    {
        vars.push_back(CompileVar(functions[curChunk].vars[o.op2].name, curCF.valStackMin + o.op1));
        break;
    }
    case Opcode::VAR_A:
    {
        // vars[curCF.varListMin + o.op1].index = stack.Size() - 1;
        CompileConst value = stack.Top();
        // std::cout << "assignment value: " << value << std::endl;
        size_t indexOfAssginee = vars[curCF.varListMin + o.op1].index;
        // std::cout << "current value there: " << stack[indexOfAssginee] << std::endl;
        stack[indexOfAssginee] = value;
        // std::cout << "Value after assignment: " << stack[indexOfAssginee] << std::endl;
        break;
    }
    // returns the value of the variable at o.op1's location + varOffset
    case Opcode::GET_V:
    {
        //     std::cout << "varListMin: " << curCF.varListMin <<  std::endl;
        //     std::cout << "GET_V index into vars: " << o.op1 + curCF.varListMin << std::endl;
        CompileVar var = vars[o.op1 + curCF.varListMin];
        // std::cout << "Var at that index: " << var << std::endl;
        // std::cout << "GET_V index into value stack: " << var.index << std::endl;
        CompileConst v = stack[var.index];
        // std::cout << "Value at that index: " << v << std::endl;
        std::cout << "Var val: " << v << std::endl;

        // std::cout << std::endl;
        // std::cout << "Stuff in vars: " << std::endl;
        // for (CompileVar &cv : vars)
        // {
        //     std::cout << cv << std::endl;
        // }

        // std::cout << std::endl;
        // std::cout << "Stuff in stack" << std::endl;
        // PrintStack();

        // std::cout << std::endl
        //           << std::endl;

        stack.Push(v);
        break;
    }
    case Opcode::DEL_V:
    {
        vars.pop_back();
        break;
    }
    // adds the operand to the ip if the value on the top of the stack is not truthy
    case Opcode::JUMP_IF_FALSE:
    {
        if (!IsTruthy(stack.Top()))
            ip += o.op1;
        // stack.Pop();
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
        // std::cout << "CALL_F, vars.size(): " << vars.size() << std::endl;
        // std::cout << "Stuff in vars" << std::endl;
        // for (CompileVar &cv : vars)
        // {
        //     std::cout << cv << std::endl;
        // }

        std::cout << "Going into function: " << o.op1 << std::endl;

        cs.Push({ip + 1, curChunk, stack.Size() - o.op2, vars.size()});

        // std::cout << "new valStackMin: " << stack.Size() - o.op2 <<std::endl;
        // std::cout << "new valListMin: " << vars.size() << std::endl;

        // std::cout << std::endl
        //           << std::endl;

        curChunk = o.op1;
        curCF = cs.Top();
        // ip is incremented after each instruction executes so need to set it to -1
        // -1 here is SIZE_MAX but C++ standard specifies unsigned addition wraps around
        // conveniently
        ip = -1;
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