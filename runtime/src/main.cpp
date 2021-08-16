#define TEST
#include "vm.h"

#ifdef COMPILE_FOR_TEST
#define main not_main
#endif

int main()
{
    VM vm = VM::DeserialiseProgram("../lbin/add.lo");

    vm.Disasemble();
    vm.ExecuteProgram();

    return 0;
}