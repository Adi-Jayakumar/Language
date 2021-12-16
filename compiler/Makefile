CC		:= g++

# -fsanitize=leak -fsanitize=address -fsanitize=undefined
C_FLAGS := -g -std=c++17 -fsanitize=leak -fsanitize=address -fsanitize=undefined -O0 -Wall -Wextra -Wpedantic -march=native -Wl,--export-dynamic

BIN		:= bin
SRC		:= src
INCLUDE_ONE	:= include
INCLUDE_TWO := ../common
LIB		:= lib

LIBRARIES	:= -ldl -lz3

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= compiler.exe
else
EXECUTABLE	:= compiler
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE_ONE) -I$(INCLUDE_TWO) -L$(LIB) $^ -o $@ $(LIBRARIES)