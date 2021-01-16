#include "vm.h"

void VM::SetChunk(Chunk &_cur)
{
    varOffset = vars.size();
    constOffset = constants.size();
    cur = _cur;
}

void VM::PrintStack()
{
    std::stack<CompileConst> s = stack;
    while(!s.empty())
    {
        std::cout << s.top() << std::endl;
        s.pop();
    }
}

void VM::ExecuteCurrentChunk()
{
    for (Op &o : cur.code)
    {
        switch (o.code)
        {
        // pops the top value off the stack
        case Opcode::POP:
        {
            stack.pop();
            break;
        }
        // returns the constant at o.operand's location + constOffset
        case Opcode::GET_C:
        {
            CompileConst c = cur.constants[constOffset + o.operand];
            stack.push(c);
            break;
        }
        // pops the value currently on the top of the stack and assigns it to a CompileVar at o.operand's location
        case Opcode::VAR_D:
        {
            CompileConst val = stack.top();
            stack.pop();
            vars.push_back(CompileVar(cur.vars[o.operand].name, val));
            break;
        }
        // returns the value of the variable at o.operand's location + varOffset
        case Opcode::GET_V:
        {
            CompileVar v = vars[varOffset + o.operand];
            // temporary untill the 'print' function is implemented
            std::cout << "Var name: " << v.name << " Var val: " << v.val << std::endl;
            stack.push(v.val);
            break;
        }
        // adds the last 2 things on the stack
        case Opcode::ADD:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left + right);
            break;
        }
        // subtracts the last 2 things on the stack
        case Opcode::SUB:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left - right);
            break;
        }
        // multiplies the last 2 things on the stack
        case Opcode::MUL:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left * right);
            break;
        }
        // divides the last 2 things on the stack
        case Opcode::DIV:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left / right);
            break;
        }
        // does a greater than comparison on the last 2 things on the stack
        case Opcode::GT:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left > right);
            break;
        }
        // does a less than comparison on the last 2 things on the stack
        case Opcode::LT:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left < right);
            break;
        }
        // does a greater than or equal comparison on the last 2 things on the stack
        case Opcode::GEQ:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left >= right);
            break;
        }
        // does a less than or equal comparison on the last 2 things on the stack
        case Opcode::LEQ:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left <= right);
            break;
        }
        // does an equality check on the last 2 things on the stack
        case Opcode::EQ_EQ:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left == right);
            break;
        }
        // does an inequality check on the last 2 things on the stack
        case Opcode::BANG_EQ:
        {
            CompileConst right = stack.top();
            stack.pop();
            CompileConst left = stack.top();
            stack.pop();
            stack.push(left != right);
            break;
        }
        }
    }
}