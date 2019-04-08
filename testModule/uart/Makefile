CC=arm-none-linux-gnueabi-gcc
TARGET=uart-test
CFLAGS=-Wall 
OBJSRC= ../../module/uart/uart.o	\
		 uart-t.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC) -lpthread
	cp $(TARGET) /home/cc/share/tmp

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



