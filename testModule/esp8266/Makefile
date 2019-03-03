CC=arm-none-linux-gnueabi-gcc
TARGET=esp8266-test
CFLAGS=-Wall 
OBJSRC= ../uart/uart.o	\
		 esp8266.o		\
		 esp8266-t.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC) -lpthread
	cp $(TARGET) /home/cc/iTop4412/busybox/system/bin

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



