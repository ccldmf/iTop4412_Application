CC=arm-none-linux-gnueabi-gcc
TARGET=screen-test
CFLAGS=-Wall
OBJSRC= test.o \
	../../module/touchScreen/touchScreen.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC)
	cp $(TARGET) /home/cc/share/tmp

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



