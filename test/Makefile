CC		:= g++

# -fsanitize=leak -fsanitize=address -fsanitize=undefined


C_FLAGS := -std=c++17 -fsanitize=leak -fsanitize=address -fsanitize=undefined -O0 -Wall -Wextra -march=native -DCOMPILE_FOR_TEST -DVM_SILENT_DESTRUCT

BIN					:= bin
COMPILER_SRC		:= ../compiler/src
RUNTIME_SRC			:= ../runtime/src
TEST_SRC			:= src

INCLUDE_ONE			:= ../common
INCLUDE_TWO 		:= ../compiler/include
INCLUDE_THREE 		:= ../runtime/include
LIB					:= lib

LIBRARIES			:= -ldl

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= test.exe
else
EXECUTABLE	:= test
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(COMPILER_SRC)/* $(RUNTIME_SRC)/* $(TEST_SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE_ONE) -I$(INCLUDE_TWO) -I$(INCLUDE_THREE) -L$(LIB) $^ -o $@ $(LIBRARIES)