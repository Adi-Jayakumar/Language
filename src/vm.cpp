#include "vm.h"

void VM::SetChunk(Chunk &_cur)
{
    varOffset = stack.Size();
    constOffset = constants.size();
    cur = _cur;
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
    while (ip != cur.code.size())
    {
        ExecuteInstruction();
        Jump(1);
    }
}

void VM::ExecuteInstruction()
{
    // Op o = cur.code[ip];
    Op o = cur.code[ip];
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
        CompileConst c = cur.constants[o.op1];
        stack.Push(c);
        break;
    }
    // pops the value currently on the top of the stack and assigns it to a CompileVar at o.op1's location
    case Opcode::VAR_D:
    {
        // CompileConst val = stack.Top();
        // vars.push_back(CompileVar(cur.vars[o.op1].name, val));
        // vars[varOffset + o.op1].index = o.op1;
        vars.push_back(CompileVar(cur.vars[o.op2].name, o.op1));
        break;
    }
    case Opcode::VAR_A:
    {
        // CompileConst val = stack.Top();
        // std::cout << "stack size in VAR_A: " << stack.Size() << std::endl;
        // std::cout << "operand in VAR_A: " << o.op1 << std::endl;
        vars[varOffset + o.op1].index = stack.Size() - 1;
        break;
    }
    // returns the value of the variable at o.op1's location + varOffset
    case Opcode::GET_V:
    {
        CompileVar var = vars[o.op1];
        CompileConst v = stack[var.index];
        // std::cout << "index of var in 'vars' array: " << o.op1 << std::endl;
        // std::cout << "index of var on stack: " << var.index << std::endl;
        // temporary until the 'print' function is implemented
        // std::cout << "Var name: " << v.name << " Var val: " << v.val << std::endl;
        std::cout << "Var val: " << v << std::endl;
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

void VM::StepThrough()
{
    while (ip != cur.code.size())
    {
        std::cout << ToString(cur.code[ip].code) << " " << +cur.code[ip].op1 << std::endl;
        ExecuteInstruction();
        Jump(1);
    }
}