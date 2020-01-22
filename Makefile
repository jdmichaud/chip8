TARGET = chip8
TEST_TARGET = test
LIBS =
CC = gcc
CFLAGS = -std=c99 -ggdb3 -Wall -D_POSIX_C_SOURCE=200809L
#CFLAGS = -std=c99 -O3 -Wall
LDFALGS = -L./

.PHONY: default all clean

default: $(TARGET)
all: default
re: clean all

OBJECTS = main.o file.c chip8.o disassembler.o debug_chip8.o
TEST_OBJECTS = test.o
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFALGS) $(OBJECTS) $(LIBS) -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(LDFALGS) $(TEST_OBJECTS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f $(TEST_TARGET)
