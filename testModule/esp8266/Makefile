CC=arm-none-linux-gnueabi-gcc
TARGET=esp8266-test
CFLAGS=-Wall 
OBJSRC= ../../module/uart/uart.o	\
	../../module/esp8266/esp8266.o		\
		 esp8266-t.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC) -lpthread

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



