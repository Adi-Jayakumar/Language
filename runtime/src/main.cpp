#include "vm.h"

int main()
{
    VM vm = VM::DeserialiseProgram("../test.lo");

    vm.Disasemble();

    return 0;
}