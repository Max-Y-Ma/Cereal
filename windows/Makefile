# Compiler
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -Werror -g -std=c11

# Source files
SRC_DIR := core
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Build files
BUILD_DIR := bin
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Executable name
TARGET := cereal

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS) main.c
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	@rmdir $(BUILD_DIR)

# Run rule
run:
	@$(BUILD_DIR)/$(TARGET)

# PHONY targets to avoid conflicts with file names
.PHONY: all clean run
