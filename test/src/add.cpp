#include "../catch.h"

#include "vm.h"

extern void CompileFile(std::string file, std::string out);

#define GET_RESULT(file)                      \
    std::string bin = "lbin/" + file + ".lo"; \
    CompileFile(file, bin);                   \
    VM vm = VM::DeserialiseProgram(bin);      \
    vm.ExecuteProgram();                      \
    Object *result = vm.stack[0]

TEST_CASE("Testing addition operator amongs intergers, doubles and strings")
{

    SECTION("1 + 1 = 2")
    {
        GET_RESULT(std::string("add"));
        REQUIRE(GetInt(result) == 2);
    }
}