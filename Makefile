CC       := gcc
CFLAGS   := -std=c11 -Wall -Wextra -pthread
LDFLAGS  :=

# Target binary
TARGET   := tcp_echo_server

# Source files and object files
SRCS     := main.c server.c
OBJS     := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile
%.o: %.c server.h
	$(CC) $(CFLAGS) -c $< -o $@

# Remove binaries and objects
clean:
	rm -f $(OBJS) $(TARGET)