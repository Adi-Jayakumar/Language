#include "vm.h"

int main()
{
    VM vm = VM::DeserialiseProgram("../test.lo");

    vm.Disasemble();

    vm.ExecuteProgram();
    // std::cout << vm.cs.size() << std::endl;
    // // for (auto &ti : vm.)

    return 0;
}