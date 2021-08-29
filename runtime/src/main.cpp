#define TEST
#include "argparser.h"
#include "vm.h"

#ifdef COMPILE_FOR_TEST
#define main not_main
#endif

int main(int argc, char **argv)
{
    ArgParser arg;
    arg.AddArg({"-f"});
    arg.ParseArgs(argc - 1, argv + 1);

    std::string binary = arg.GetArgVal("-f");
    VM vm = VM::DeserialiseProgram(binary);

    vm.Disasemble();
    vm.ExecuteProgram();

    return 0;
}