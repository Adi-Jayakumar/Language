#include "vm.h"

VM::VM(std::vector<Function> &_functions, size_t mainIndex,
       std::unordered_map<size_t, std::unordered_set<size_t>> &_StructTree,
       std::vector<LibraryFunctionDef> &_syms, std::vector<ThrowInfo> &_throwInfos)
{
    functions = _functions;
    StructTree = _StructTree;
    throwInfos = _throwInfos;

    for (auto &lf : _syms)
    {
        std::string libpath = "./lib/lib" + lf.library + ".so";
        void *handle = dlopen(libpath.c_str(), RTLD_NOW);
        libHandles.push_back(handle);

        LibFunc func;
        *(void **)&func = dlsym(handle, lf.name.c_str());
        CLibs.push_back({func, lf.arity});
    }

    curFunc = mainIndex == UINT8_MAX ? UINT8_MAX : 0;
    cs.push_back({0, mainIndex, 0});
    // cs.push({0, mainIndex, 0});
    curCF = &cs.back();

    ip = 0;
}

VM::~VM()
{
    if (libHandles.size() > 0)
    {
        std::cout << "Closing libraries" << std::endl;
        for (auto &handle : libHandles)
            dlclose(handle);
    }

    std::cout << "====================HEAP====================" << std::endl
              << std::endl
              << std::endl;

    for (size_t i = 0; i < heap.Size(); i++)
        std::cout << (heap[i]->state != GCState::FREED ? heap[i]->ToString() : "FREED") << std::endl;

    std::cout << std::endl
              << std::endl;

    for (size_t i = 0; i < heap.Size(); i++)
    {
        if (heap[i]->state != GCState::FREED)
            heap[i]->DestroyOwnedMemory();
        delete heap[i];
    }
}

void VM::Disasemble()
{
    for (size_t i = 0; i < functions.size(); i++)
    {
        std::cout << "Function index: " << i << std::endl
                  << "Function arity: " << +functions[i].arity
                  << std::endl
                  << std::endl;

        functions[i].PrintCode();

        std::cout << std::endl
                  << std::endl;
    }
}

void VM::PrintStack()
{
    std::cout << "index\t|\tvalue" << std::endl;
    for (size_t i = stack.count - 1; (int)i >= 0; i--)
        std::cout << i << "\t|\t" << stack[i]->ToString() << std::endl;

    std::cout << std::endl
              << std::endl;
}

void VM::PrintCallStack()
{
    for (auto &cf : cs)
        std::cout << "(" << cf.retIndex << ", " << cf.retFunction << ", " << cf.valStackMin << ")" << std::endl;
}

void VM::AddToHeap(Object **objs, size_t numObjs)
{
    for (size_t i = 0; i < numObjs; i++)
        heap.AddObject(objs[i]);
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
    if (curFunc == UINT8_MAX)
        return;
    while (true)
    {

        while (ip < functions[curFunc].code.size())
        {
            ExecuteInstruction();
            Jump(1);
        }

        if (!cs.empty())
        {

            CallFrame returnCF = *curCF;
            cs.pop_back();
            curCF = &cs.back();
            ip = returnCF.retIndex;

            if (curFunc != 0)
                ip++;

            curFunc = returnCF.retFunction;
            size_t stackDiff = stack.count - returnCF.valStackMin;

            // cleaning up the function's constants
            stack.count -= stackDiff;
            stack.back = stack[stack.count];
        }
        else
            return;
    }
}

#define BINARY_I_OP(l, op, r) \
    CreateInt(GetInt(l) op GetInt(r))

#define BINARY_DI_OP(l, op, r) \
    CreateDouble(GetDouble(l) op GetInt(r))

#define BINARY_ID_OP(l, op, r) \
    CreateDouble(GetInt(l) op GetDouble(r))

#define BINARY_D_OP(l, op, r) \
    CreateDouble(GetDouble(l) op GetDouble(r))

#define UNARY_I_OP(op, r) \
    CreateInt(op GetInt(r))

#define UNARY_D_OP(op, r) \
    CreateDouble(op GetDouble(r))

#define UNARY_B_OP(op, r) \
    CreateBool(op r->as.b)

#define TAKE_LEFT_RIGHT(left, right, stack) \
    right = stack.back;                     \
    stack.pop_back();                       \
    left = stack.back;                      \
    stack.pop_back()

void VM::ExecuteInstruction()
{
    Op o = functions[curFunc].code[ip];
    switch (o.code)
    {
    case Opcode::POP:
    {
        stack.pop_back();
        break;
    }
    case Opcode::LOAD_INT:
    {
        int i = functions[curFunc].ints[o.op];
        Object *obj = CreateInt(i);
        heap.AddObject(obj);
        stack.push_back(obj);
        break;
    }
    case Opcode::LOAD_DOUBLE:
    {
        double d = functions[curFunc].doubles[o.op];
        Object *obj = CreateDouble(d);
        heap.AddObject(obj);
        stack.push_back(obj);
        break;
    }
    case Opcode::LOAD_BOOL:
    {

        bool b = functions[curFunc].bools[o.op];
        Object *obj = CreateBool(b);
        heap.AddObject(obj);
        stack.push_back(obj);
        break;
    }
    case Opcode::LOAD_STRING:
    {
        std::string str = functions[curFunc].strings[o.op];
        char *c = new char[str.length() + 1];
        stack.push_back(CreateString(strcpy(c, str.c_str()), str.length()));
        break;
    }
    case Opcode::LOAD_CHAR:
    {
        char c = functions[curFunc].chars[o.op];
        stack.push_back(CreateChar(c));
        break;
    }
    case Opcode::VAR_A:
    {
        stack.data[o.op + curCF->valStackMin] = stack.back;
        break;
    }
    case Opcode::VAR_A_GLOBAL:
    {
        // globals[o.op] = stack.back;
        break;
    }
    case Opcode::VAR_D_GLOBAL:
    {
        // globals.push_back(stack.back);
        break;
    }
    case Opcode::GET_V:
    {
        stack.push_back(stack[o.op + curCF->valStackMin]);
        break;
    }
    case Opcode::GET_V_GLOBAL:
    {
        // stack.push_back(globals[o.op]);
        break;
    }
    case Opcode::ARR_D:
    {
        Object *array = stack[stack.count - o.op - 1];
        Object **data = GetArray(array);

        for (size_t i = 0; i < o.op; i++)
            data[i] = stack[stack.count - o.op + i];

        stack.pop_N(o.op);
        break;
    }
    case Opcode::ARR_INDEX:
    {
        Object *index = stack.back;
        stack.pop_back();
        Object *obj = stack.back;

        if (IsNull_T(obj))
            RuntimeError("Cannot index into an uninitialised array");

        Object **data = GetArray(obj);
        stack.pop_back();

        size_t size = GetArrayLength(obj);
        int i = GetInt(index);

        if (i >= (int)size || i < 0)
            RuntimeError("Array i " + std::to_string(i) + " out of bounds for array of size " + std::to_string(size));

        stack.push_back(data[i]);
        break;
    }
    case Opcode::ARR_SET:
    {
        Object *index = stack.back;
        stack.pop_back();
        int i = GetInt(index);

        Object *obj = stack.back;
        Object **data = GetArray(obj);
        stack.pop_back();

        size_t size = GetArrayLength(obj);
        if (i >= (int)size || i < 0)
            RuntimeError("Array index " + std::to_string(i) + " out of bounds for array of size " + std::to_string(size));

        data[i] = stack.back;
        break;
    }
    case Opcode::ARR_ALLOC:
    {
        size_t size = o.op;
        Object **data = new Object *[size];
        Object *arr = CreateArray(data, size);

        for (size_t i = 0; i < size; i++)
            data[i] = CreateNull_T();

        stack.push_back(arr);
        break;
    }
    case Opcode::STRUCT_ALLOC:
    {
        size_t size = o.op;
        Object **data = new Object *[size];
        Object *strct = CreateStruct(data, size, 1);

        stack.push_back(strct);
        break;
    }
    case Opcode::STRING_INDEX:
    {
        Object *indexObj = stack.back;
        stack.pop_back();
        Object *obj = stack.back;

        if (IsNull_T(obj))
            RuntimeError("Cannot index into an uninitialised array");
        stack.pop_back();

        char *str = GetString(obj);

        size_t size = GetStringLen(obj);
        int index = GetInt(indexObj);

        if (index >= (int)size || index < 0)
            RuntimeError("String index " + std::to_string(index) + " out of bounds for array of size " + std::to_string(size));

        stack.push_back(CreateChar(str[index]));
        break;
    }
    case Opcode::STRING_SET:
    {
        Object *index = stack.back;
        stack.pop_back();
        int i = GetInt(index);

        Object *strObj = stack.back;
        stack.pop_back();

        char *str = GetString(strObj);
        size_t strLen = GetStringLen(strObj);

        if (i >= (int)strLen || i < 0)
            RuntimeError("String index " + std::to_string(i) + " out of bounds for string of size " + std::to_string(strLen));

        str[i] = GetChar(stack.back);
        break;
    }
    case Opcode::JUMP_IF_FALSE:
    {
        if (!stack.back->IsTruthy())
            ip += o.op;
        stack.pop_back();
        break;
    }
    case Opcode::JUMP:
    {
        ip += o.op;
        break;
    }
    case Opcode::SET_IP:
    {
        ip = o.op;
        break;
    }
    case Opcode::CALL_F:
    {
        cs.push_back({ip, curFunc, stack.count - functions[o.op].arity});
        curCF = &cs.back();

        if (cs.size() > STACK_MAX)
            RuntimeError("CallStack overflow. Used: " + std::to_string(cs.size()) + " call-frams");

        curFunc = o.op;
        ip = -1;
        break;
    }
    case Opcode::CALL_LIBRARY_FUNC:
    {
        std::pair<LibFunc, size_t> func = CLibs[o.op];
        Object **args = &stack.data[stack.count - func.second];
        Object *result = func.first(this, args);

        stack.pop_N(func.second);
        if (result != nullptr)
            stack.push_back(result);

        break;
    }
    case Opcode::RETURN:
    {
        CallFrame returnCF = *curCF;
        cs.pop_back();
        curCF = &cs.back();

        ip = returnCF.retIndex;
        curFunc = returnCF.retFunction;

        size_t stackDiff = stack.count - returnCF.valStackMin;
        Object *retVal;
        retVal = stack.back;

        // cleaning up the function's constants
        stack.pop_N(stackDiff);
        stack.push_back(retVal);
        break;
    }
    case Opcode::RETURN_VOID:
    {
        CallFrame returnCF = *curCF;
        cs.pop_back();
        curCF = &cs.back();

        ip = returnCF.retIndex;
        curFunc = returnCF.retFunction;

        size_t stackDiff = stack.count - returnCF.valStackMin;

        // cleaning up the function's constants
        stack.pop_N(stackDiff);
        break;
    }
    case Opcode::PUSH_THROW_INFO:
    {
        ThrowStack.push(throwInfos[o.op]);
        break;
    }
    case Opcode::THROW:
    {
        ThrowObject();
        break;
    }
    case Opcode::NATIVE_CALL:
    {
        switch (o.op)
        {
        case 1:
        {
            NativeToString();
            break;
        }
        default:
            break;
        }
        break;
    }
    case Opcode::PRINT:
    {
        NativePrint(o.op);
        break;
    }
    case Opcode::STRUCT_MEMBER:
    {
        Object *strct = stack.back;
        if (IsNull_T(strct))
            RuntimeError("Cannot access into a null struct");
        stack.pop_back();
        stack.push_back(GetStructMembers(strct)[o.op]);
        break;
    }
    case Opcode::STRUCT_D:
    {
        Object *arr = stack[stack.count - o.op - 2];

        Object *type = stack[stack.count - o.op - 1];
        Object **data = GetStructMembers(arr);

        SetStructType(arr, static_cast<TypeID>(GetInt(arr)));

        for (size_t i = 0; i < o.op; i++)
            data[i] = stack[stack.count - o.op + i];

        stack.pop_N(o.op);
        break;
    }
    case Opcode::STRUCT_MEMBER_SET:
    {
        Object *strct = stack.back;

        if (IsNull_T(strct))
            RuntimeError("Cannot set a member of a null struct");
        stack.pop_back();
        GetStructMembers(strct)[o.op] = stack.back;
        break;
    }
    case Opcode::CAST:
    {
        Object *obj = stack.back;
        stack.pop_back();
        if (IsNull_T(obj))
            RuntimeError("Cannot cast a null value");
        else if (dynamic_cast<Int *>(obj) != nullptr)
        {
            stack.push_back(CreateDouble((double)GetInt(obj)));
            break;
        }
        else if (dynamic_cast<Double *>(obj) != nullptr)
        {
            stack.push_back(CreateInt((int)GetDouble(obj)));
            break;
        }
        else if (dynamic_cast<Struct *>(obj) != nullptr)
        {
            size_t type = GetStructType(obj);
            if (StructTree[o.op].find(type) == StructTree[o.op].end() && type != o.op)
            {
                stack.pop_back();
                stack.push_back(CreateNull_T());
            }
            break;
        }
        break;
    }
    // ADDITIONS: adds the last 2 things on the stack
    case Opcode::I_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, +, right));
        break;
    }
    case Opcode::DI_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, +, right));
        break;
    }
    case Opcode::ID_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, +, right));
        break;
    }
    case Opcode::D_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, +, right));
        break;
    }
    case Opcode::S_ADD:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        char *lStr = GetString(left);
        char *rStr = GetString(right);

        size_t leftSize = GetStringLen(left);
        size_t rightSize = GetStringLen(right);
        size_t newStrSize = leftSize + rightSize;

        char *concat = new char[newStrSize + 1];
        strcpy(concat, lStr);

        char *next = concat + leftSize;
        strcpy(next, rStr);

        stack.push_back(CreateString(concat, newStrSize));
        break;
    }
    // SUBTRACTIONS: subtracts the last 2 things on the stack
    // if o.op is 1 then is a unary negation (only the case
    // for I_SUB and D_SUB obviously)
    case Opcode::I_SUB:
    {
        Object *right = stack.back;
        stack.pop_back();
        if (o.op == 0)
        {
            Object *left = stack.back;
            stack.pop_back();

            stack.push_back(CreateInt(GetInt(left) - GetInt(right)));
        }
        else
            stack.push_back(CreateInt(-GetInt(right)));
        break;
    }
    case Opcode::DI_SUB:
    {
        // DI_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);

        stack.push_back(BINARY_DI_OP(left, -, right));
        break;
    }
    case Opcode::ID_SUB:
    {
        // ID_SUB cannot be a unary operation
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);

        stack.push_back(BINARY_ID_OP(left, -, right));
        break;
    }
    case Opcode::D_SUB:
    {
        Object *right = stack.back;
        stack.pop_back();

        if (o.op == 0)
        {
            Object *left = stack.back;
            stack.pop_back();

            stack.push_back(CreateDouble(GetDouble(left) - GetDouble(right)));
        }
        else
            stack.push_back(CreateDouble(-GetDouble(right)));
        break;
    }
    // multiplies the last 2 things on the stack
    case Opcode::I_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, *, right));
        break;
    }
    case Opcode::DI_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, *, right));
        break;
    }
    case Opcode::ID_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, *, right));
        break;
    }
    case Opcode::D_MUL:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, *, right));
        break;
    }
    // divides the last 2 things on the stack
    case Opcode::I_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, /, right));
        break;
    }
    case Opcode::DI_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, /, right));
        break;
    }
    case Opcode::ID_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, /, right));
        break;
    }
    case Opcode::D_DIV:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, /, right));
        break;
    }
    // does a greater than comparison on the last 2 things on the stack
    case Opcode::I_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, >, right));
        break;
    }
    case Opcode::DI_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, >, right));
        break;
    }
    case Opcode::ID_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, >, right));
        break;
    }
    case Opcode::D_GT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, >, right));
        break;
    }
    // does a less than comparison on the last 2 things on the stack
    case Opcode::I_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, <, right));
        break;
    }
    case Opcode::DI_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, <, right));
        break;
    }
    case Opcode::ID_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, <, right));
        break;
    }
    case Opcode::D_LT:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, <, right));
        break;
    }
    // does a greater than or equal comparison on the last 2 things on the stack
    case Opcode::I_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, >=, right));
        break;
    }
    case Opcode::DI_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, >=, right));
        break;
    }
    case Opcode::ID_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, >=, right));
        break;
    }
    case Opcode::D_GEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, >=, right));
        break;
    }
    // does a less than or equal comparison on the last 2 things on the stack
    case Opcode::I_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, <=, right));
        break;
    }
    case Opcode::DI_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, <=, right));
        break;
    }
    case Opcode::ID_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, <=, right));
        break;
    }
    case Opcode::D_LEQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, <=, right));
        break;
    }
    case Opcode::N_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(IsNull_T(left) && IsNull_T(right)));
        break;
    }
    // does an equality check on the last 2 things on the stack
    case Opcode::I_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, ==, right));
        break;
    }
    case Opcode::DI_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, ==, right));
        break;
    }
    case Opcode::ID_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, ==, right));
        break;
    }
    case Opcode::D_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, ==, right));
        break;
    }
    case Opcode::B_EQ_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(GetBool(left) == GetBool(right)));
        break;
    }
    case Opcode::N_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(!IsNull_T(left) || !IsNull_T(right)));
        break;
    }
    // does an inequality check on the last 2 things on the stack
    case Opcode::I_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_I_OP(left, !=, right));
        break;
    }
    case Opcode::DI_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_DI_OP(left, !=, right));
        break;
    }
    case Opcode::ID_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_ID_OP(left, !=, right));
        break;
    }
    case Opcode::D_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(BINARY_D_OP(left, !=, right));
        break;
    }
    case Opcode::B_BANG_EQ:
    {
        TAKE_LEFT_RIGHT(Object * left, Object * right, stack);
        stack.push_back(CreateBool(GetBool(left) != GetBool(right)));
        break;
    }
    case Opcode::BANG:
    {
        Object *right = stack.back;
        stack.pop_back();
        stack.push_back(CreateBool(!GetBool(right)));
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
        std::cout << stack[stack.count - arity + i]->ToString();

    std::cout << std::endl;
    stack.pop_N((size_t)arity);
}

void VM::NativeToString()
{
    std::string str = stack.back->ToString();
    stack.pop_back();
    char *c = new char[str.length() + 1];
    stack.push_back(new String(strcpy(c, str.c_str()), str.length()));
}

void VM::ThrowObject()
{
    Object *throwObj = stack.back;
    stack.pop_back();

    while (!ThrowStack.empty())
    {
        if (MatchType(throwObj, ThrowStack.top().isArray, ThrowStack.top().type))
        {
            ThrowInfo loc = ThrowStack.top();
            ThrowStack.pop();
            curFunc = loc.func;
            ip = loc.index - 1;

            cs.erase(cs.begin() + loc.callStackIndex, cs.end());
            curCF = &cs[loc.callStackIndex];

            stack.count = cs[loc.callStackIndex].valStackMin;
            stack.back = stack[stack.count];

            stack.push_back(throwObj);
            return;
        }
        ThrowStack.pop();
    }
    RuntimeError("Uncaught throw");
}

bool VM::MatchType(Object *obj, size_t isArray, uint8_t type)
{
    Int *i = dynamic_cast<Int *>(obj);
    if (i != nullptr)
        return (isArray == 0) && (type == 1);

    Double *d = dynamic_cast<Double *>(obj);
    if (d != nullptr)
        return (isArray == 0) && (type == 2);

    Bool *b = dynamic_cast<Bool *>(obj);
    if (b != nullptr)
        return (isArray == 0) && (type == 3);

    Struct *s = dynamic_cast<Struct *>(obj);
    if (s != nullptr)
        return (isArray == 0) && (type == GetStructType(obj));

    Char *c = dynamic_cast<Char *>(obj);
    if (c != nullptr)
        return (isArray == 0) && (type == 5);

    String *str = dynamic_cast<String *>(obj);
    if (str != nullptr)
        return (isArray == 0) && (type == 4);

    return false;
}

VM VM::DeserialiseProgram(std::string fPath)
{
    std::ifstream file;
    if (!DoesFileExist(fPath))
        DeserialisationError("File '" + fPath + "' does not exist");

    file.open(fPath, std::ios::in | std::ios::binary);

    uint8_t mainIndex;
    file.read((char *)&mainIndex, sizeof(uint8_t));

    uint8_t numFunctions;
    file.read((char *)&numFunctions, sizeof(uint8_t));

    std::vector<Function> program;
    for (uint8_t i = 0; i < numFunctions; i++)
        program.push_back(DeserialiseFunction(file));

    std::unordered_map<size_t, std::unordered_set<size_t>> StructTree;
    std::vector<LibraryFunctionDef> libFuncs;
    std::vector<ThrowInfo> throwInfos;

    while (file.peek() != EOF)
    {
        size_t id = ReadSizeT(file);
        switch (id)
        {
        case STRUCT_TREE_ID:
        {
            size_t numStructs = ReadSizeT(file);

            for (size_t i = 0; i < numStructs; i++)
            {
                size_t structId = ReadSizeT(file);
                size_t numParents = ReadSizeT(file);

                for (size_t i = 0; i < numParents; i++)
                    StructTree[structId].insert(ReadSizeT(file));
            }

            break;
        }
        case LIB_FUNC_ID:
        {
            size_t numLibFuncs = ReadSizeT(file);

            for (size_t i = 0; i < numLibFuncs; i++)
            {
                size_t nameLen = ReadSizeT(file);
                char *cName = (char *)DeserialiseData(nameLen, sizeof(char), file);
                std::string name(cName, nameLen);
                delete[] cName;

                size_t libLen = ReadSizeT(file);
                char *cLibName = (char *)DeserialiseData(libLen, sizeof(char), file);
                std::string libName(cLibName, libLen);
                delete[] cLibName;

                size_t arity = ReadSizeT(file);
                libFuncs.push_back(LibraryFunctionDef(name, libName, arity));
            }
            break;
        }
        case THROW_INFO_ID:
        {
            throwInfos = DeserialiseThrowInfos(file);
            break;
        }
        default:
        {
            DeserialisationError("Invalid section identifier " + std::to_string(id));
            break;
        }
        }
    }
    file.close();
    return VM(program, mainIndex, StructTree, libFuncs, throwInfos);
}

bool VM::DoesFileExist(std::string &path)
{
    return std::ifstream(path).good();
}

void VM::DeserialisationError(std::string err)
{
    Error e = Error("[DE-SERIALISATION ERROR]\n" + err + "\n");
    throw e;
}

Function VM::DeserialiseFunction(std::ifstream &file)
{
    uint8_t arity;
    file.read((char *)&arity, sizeof(uint8_t));

    std::vector<int> Ints;
    std::vector<double> Doubles;
    std::vector<bool> Bools;
    std::vector<char> Chars;
    std::vector<std::string> Strings;
    std::vector<Op> Code;

    for (size_t i = 0; i < 6; i++)
    {
        size_t typeCode = ReadSizeT(file);
        switch (typeCode)
        {
        case INT_ID:
        {
            Ints = DeserialiseInts(file);
            break;
        }
        case DOUBLE_ID:
        {
            Doubles = DeserialiseDoubles(file);
            break;
        }
        case BOOL_ID:
        {
            Bools = DeserialiseBools(file);
            break;
        }
        case CHAR_ID:
        {
            Chars = DeserialiseChars(file);
            break;
        }
        case STRING_ID:
        {
            Strings = DeserialiseStrings(file);
            break;
        }
        case CODE_ID:
        {
            Code = DeserialiseOps(file);
            break;
        }
        }
    }

    return Function(arity, Code, Ints, Doubles, Bools, Chars, Strings);
}

//=================================DE-SERIALISATION=================================//

size_t VM::ReadSizeT(std::ifstream &file)
{
    char cNumElements[sizeof(size_t)];
    file.read(cNumElements, sizeof(size_t));

    return *(size_t *)cNumElements;
}

void *VM::DeserialiseData(size_t numElements, size_t typeSize, std::ifstream &file)
{
    char *cData = new char[typeSize * numElements];

    file.read(cData, typeSize * numElements);
    return cData;
}

std::vector<int> VM::DeserialiseInts(std::ifstream &file)
{
    size_t numInts = ReadSizeT(file);
    int *data = (int *)DeserialiseData(numInts, sizeof(int), file);

    std::vector<int> result(data, data + numInts);
    delete[] data;
    return result;
}

std::vector<double> VM::DeserialiseDoubles(std::ifstream &file)
{
    size_t numDoubles = ReadSizeT(file);
    double *data = (double *)DeserialiseData(numDoubles, sizeof(double), file);

    std::vector<double> result(data, data + numDoubles);
    delete[] data;
    return result;
}

std::vector<bool> VM::DeserialiseBools(std::ifstream &file)
{
    size_t numBools = ReadSizeT(file);
    bool *data = (bool *)DeserialiseData(numBools, sizeof(bool), file);

    std::vector<bool> result(data, data + numBools);
    delete[] data;
    return result;
}

std::vector<char> VM::DeserialiseChars(std::ifstream &file)
{
    size_t numChars = ReadSizeT(file);
    char *data = (char *)DeserialiseData(numChars, sizeof(char), file);

    std::vector<char> result(data, data + numChars);
    delete[] data;
    return result;
}

std::vector<std::string> VM::DeserialiseStrings(std::ifstream &file)
{
    size_t numStrings = ReadSizeT(file);
    std::vector<std::string> result;

    for (size_t i = 0; i < numStrings; i++)
    {
        size_t strLen = ReadSizeT(file);
        char *data = (char *)DeserialiseData(strLen, sizeof(char), file);
        result.push_back(std::string(data, strLen));
        delete[] data;
    }

    return result;
}

std::vector<Op> VM::DeserialiseOps(std::ifstream &file)
{
    size_t numOps = ReadSizeT(file);
    std::vector<Op> result;

    for (size_t i = 0; i < numOps; i++)
    {
        void *vCode = DeserialiseData(1, sizeof(Opcode), file);
        Opcode code = static_cast<Opcode>(*(uint8_t *)vCode);
        delete[](char *) vCode;

        void *vOprand = DeserialiseData(1, sizeof(uint8_t), file);
        uint8_t oprand = static_cast<uint8_t>(*(uint8_t *)vOprand);
        delete[](char *) vOprand;

        result.push_back(Op(code, oprand));
    }

    return result;
}

std::vector<ThrowInfo> VM::DeserialiseThrowInfos(std::ifstream &file)
{
    size_t numThrows = ReadSizeT(file);
    std::vector<ThrowInfo> result;

    for (size_t i = 0; i < numThrows; i++)
    {
        ThrowInfo ti = ThrowInfo();
        file.read((char *)&ti.isArray, sizeof(bool));
        file.read((char *)&ti.type, sizeof(uint8_t));
        file.read((char *)&ti.func, sizeof(uint8_t));
        file.read((char *)&ti.index, sizeof(uint8_t));
        result.push_back(ti);
    }
    return result;
}