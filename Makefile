CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -I./src
SRC = src/main.c \
      src/util.c \
      src/ast/ast.c \
      src/lexer/lexer.c \
      src/parser/parser.c \
      src/semantic/semantic.c \
      src/symbol_table/symbol_table.c \
      src/tac/tac.c

TARGET = compiler
FILE ?= tests/valid/complete_program.md

.PHONY: all build run clean

all: build

build: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(FILE)

clean:
	rm -f $(TARGET) *.o
