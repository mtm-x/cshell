# --- Variables ---
# The compiler to use
CC = gcc

# Compiler flags:
# -Wall -Wextra: Enable most warnings (good for quality)
# -g: Add debug info (for GDB/Valgrind)
# -Iinclude: Look for headers in the 'include' folder
CFLAGS = -Wall -Wextra -g -Iinclude

# The final executable name
TARGET = bin/cshell

# Source files: Find all .c files in src/
SRCS = $(wildcard src/*.c)

# Object files: Convert src/filename.c -> obj/filename.o
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))

# --- Rules ---

# Default rule: Build the target
all: $(TARGET)

# Link: Create the executable from object files
$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Build successful: $@"

# Compile: Create object files from source files
obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

git:
	git add .
	git commit -m "$(commit)"
# Clean: Remove build artifacts
clean:
	rm -rf obj bin

# Run: Build and run the program
run: all
	./$(TARGET)

# Phony targets (not actual files)
.PHONY: all clean run
