# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror -g

# Source files
SRCS = main.c my_shell.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = my_shell

# Default rule
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.c my_shell.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and the target
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
