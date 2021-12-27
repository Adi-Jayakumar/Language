CC		:= g++
C_FLAGS := -g -std=c++17 -Wall -Wextra -Wpedantic -march=native -fext-numeric-literals
ASAN_FLAGS := -fsanitize=leak -fsanitize=address -fsanitize=undefined

BIN		:= bin
SRC		:= src
OBJ		:= obj
TEST	:= test
INCLUDE_ONE	:= include
INCLUDE_TWO := ../common
INCLUDE_PATHS := -I$(INCLUDE_ONE) -I$(INCLUDE_TWO)

SRC_FILES := $(wildcard $(SRC)/*.cpp)
OBJ_FILES := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRC_FILES))

LIBRARIES	:= -lz3

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= compiler.exe
else
EXECUTABLE	:= compiler
endif

all: $(BIN)/$(EXECUTABLE)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(C_FLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(BIN)/$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(ASAN_FLAGS) $^ -o $@ $(LIBRARIES)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)