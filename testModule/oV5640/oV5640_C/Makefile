CC=arm-none-linux-gnueabi-gcc
TARGET=camera-test
CFLAGS=-Wall
OBJSRC= main.o \
	../../../module/oV5640/oV5640_C/camera.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC)
	cp $(TARGET) /home/cc/share/tmp

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



