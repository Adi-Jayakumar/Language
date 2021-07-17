#include "vm.h"

int main()
{
    VM vm = VM::DeserialiseProgram("../test.lo");

    vm.Disasemble();

    vm.ExecuteProgram();

    // for (auto &ti : vm.)

    return 0;
}