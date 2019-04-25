CC=arm-none-linux-gnueabi-gcc
TARGET=ucGUI-test
CFLAGS=-Wall 
OBJSRC= test.o
LIB=libucgui.a

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC) $(LIB)
	cp $(TARGET) /home/cc/share/tmp

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



