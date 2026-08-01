.SUFFIXES:

CC ?= gcc
PYTHON ?= python3

CPPFLAGS := -I./src
CFLAGS ?= -std=c11 -O2
WARNINGS := -Wall -Wextra -Wpedantic

SOURCES := \
	src/main.c \
	src/util.c \
	src/ast/ast.c \
	src/lexer/lexer.c \
	src/parser/parser.c \
	src/semantic/semantic.c \
	src/symbol_table/symbol_table.c \
	src/tac/tac.c

OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
TARGET := compiler
FILE ?= tests/valid/complete_program.md

.PHONY: all build run test expected sanitize clean distclean flex-bison-check

all: build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(WARNINGS) $(OBJECTS) -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) -MMD -MP -c $< -o $@

-include $(OBJECTS:.o=.d)

run: $(TARGET)
	./$(TARGET) $(FILE)

test: $(TARGET)
	$(PYTHON) scripts/run_tests.py

expected: $(TARGET)
	$(PYTHON) scripts/run_tests.py --update-expected

sanitize:
	$(CC) $(CPPFLAGS) -std=c11 -O1 -g $(WARNINGS) \
		-fsanitize=address,undefined -fno-omit-frame-pointer \
		$(SOURCES) -o compiler-sanitize
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 ./compiler-sanitize $(FILE)

# The repository includes formal Flex/Bison specifications for course review.
# This target validates that the generator tools are installed and regenerates
# files into build/generated without replacing the tested portable build.
flex-bison-check:
	@command -v flex >/dev/null || { echo "flex is not installed"; exit 1; }
	@command -v bison >/dev/null || { echo "bison is not installed"; exit 1; }
	@mkdir -p build/generated
	bison -Wall -Wcounterexamples -d -o build/generated/parser.tab.c src/parser/parser.y
	flex -o build/generated/lexer.yy.c src/lexer/lexer.l
	@echo "Flex/Bison sources generated successfully in build/generated/."

clean:
	rm -rf build $(TARGET) compiler-sanitize compiler_san

distclean: clean
	rm -rf tests/expected/valid/*.txt tests/expected/invalid/*.txt
