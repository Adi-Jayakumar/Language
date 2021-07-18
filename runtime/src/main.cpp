#include "vm.h"

int main()
{
    VM vm = VM::DeserialiseProgram("../test.lo");

    vm.Disasemble();

    std::cout << "THROW STACK SIZE " << vm.throwInfos.size() << std::endl;
    for (auto &ti : vm.throwInfos)
    {
        std::cout << "TrowInfos(" << +ti.func << ", " << +ti.index << ", " << ti.isArray << ", " << +ti.type << ", " << ti.callStackIndex << ")" << std::endl;
    }

    return 0;
}