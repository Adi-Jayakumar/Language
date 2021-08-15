#define TEST
#include "vm.h"

int main()
{
    VM vm = VM::DeserialiseProgram("../lbin/test.lo");

    vm.Disasemble();
    vm.ExecuteProgram();

    return 0;
}