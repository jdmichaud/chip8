LIB_TARGET = libchip8.a
DIS_TARGET = d8
CHIP_TARGET = c8
CC = gcc
AR = ar
CFLAGS = -std=c99 -ggdb3 -Wall -fstack-protector-strong					\
	-D_POSIX_C_SOURCE=200809L -DSUPPORT_FILEFORMAT_BMP 						\
	-Iraylib-2.5.0-Linux-amd64/include/

.PHONY: default all clean

default: $(LIB_TARGET)
all: default
re: clean all

HEADERS = $(wildcard *.h)

LIB_SRCS = chip8.c disassemble.c debug_chip8.c
LIB_OBJS = $(patsubst %.c,%.o,$(LIB_SRCS))

DIS_SRCS = d8.c file.c
DIS_OBJS = $(patsubst %.c,%.o,$(DIS_SRCS))
DIS_LIBS = -lchip8
DIS_LDFLAGS = -L.

CHIP_SRCS = c8.c file.c text.c
CHIP_OBJS = $(patsubst %.c,%.o,$(CHIP_SRCS))
CHIP_LIBS = -lchip8 -lraylib
CHIP_LDFLAGS = -L. -Lraylib-2.5.0-Linux-amd64/lib/

%: %.c
	$(CC) $(CFLAGS)  -o $@ $<

$(LIB_TARGET): $(LIB_OBJS)
	$(AR) rcs $(LIB_TARGET) $(LIB_OBJS)

$(DIS_TARGET): $(DIS_OBJS) $(LIB_TARGET)
	$(CC) -o $@ $(DIS_LDFLAGS) $(DIS_OBJS) $(DIS_LIBS)

$(CHIP_TARGET): $(CHIP_OBJS) $(LIB_TARGET)
	$(CC) -o $@ $(CHIP_LDFLAGS) $(CHIP_OBJS) $(CHIP_LIBS)

clean:
	-rm -f *.o
	-rm -f $(LIB_TARGET)
	-rm -f $(CHIP_TARGET)
	-rm -f $(DIS_TARGET)
	-rm -f $(TEST_TARGET)
