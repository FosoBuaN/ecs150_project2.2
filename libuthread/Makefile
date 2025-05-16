# Target library
lib := libuthread.a

all: $(lib)

# Variables
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra
ARFLAGS = rcs
objs = queue.o uthread.o context.o sem.o preempt.o

# Default target (build the static library)
all: $(lib)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Archive the static library
$(lib): $(objs)
	$(AR) $(ARFLAGS) $@ $^
