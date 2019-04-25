
CC=arm-none-linux-gnueabi-gcc
CFLAGS=-I ../Core -I ../../Config/ -I ../WM/

SRCS:=$(wildcard *.c)

OBJS:=$(SRCS:%.c=%.o)

all:$(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean

clean:
	rm *.o


