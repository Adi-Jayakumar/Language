#include "../catch.h"
#include "parser.h"
#include "staticanalyser.h"
#include "vm.h"

extern void Compile(std::vector<std::shared_ptr<Stmt>> &program, std::string out);

TEST_CASE("Testing addition operator amongs intergers, doubles and strings")
{
    Parser p("scripts/add.txt");
    StaticAnalyser s;

    std::vector<std::shared_ptr<Stmt>> program = p.Parse();
    for (auto &stmt : program)
        stmt->Type(s);

    Compile(program, "lbin/add.lo");
    VM vm = VM::DeserialiseProgram("lbin/add.lo");
    vm.ExecuteProgram();
    Object *result = vm.stack[0];

    SECTION("1 + 1 = 2")
    {
        REQUIRE(GetInt(result) == 2);
    }
}