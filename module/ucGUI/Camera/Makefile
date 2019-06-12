
CC=arm-none-linux-gnueabi-gcc
CFLAGS= -I ../../Config/

SRCS:=$(wildcard *.c)

OBJS:=$(SRCS:%.c=%.o)

all:$(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread

.PHONY:clean

clean:
	rm *.o


