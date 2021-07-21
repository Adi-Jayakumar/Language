CC		:= g++

# -fsanitize=leak -fsanitize=address -fsanitize=undefined


C_FLAGS := -std=c++17 -fsanitize=leak -fsanitize=address -fsanitize=undefined -O0 -Wall -Wextra -march=native -Wl,--export-dynamic

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= -ldl

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
else
EXECUTABLE	:= main
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)