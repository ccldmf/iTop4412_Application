CC=arm-none-linux-gnueabi-gcc
#CC=gcc
TARGET=bigiot-test
CFLAGS=-Wall
#OBJSRC= ../../module/tcp/tcp.o	\
#	    ../../module/bigiot/bigiot.o	\
#		 bigiot-test.o

OBJSRC= ../../module/uart/uart.o	\
		../../module/esp8266/esp8266.o	\
	    ../../module/bigiot/bigiot.o	\
		 bigiot-test.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC) -pthread
	cp $(TARGET) /home/cc/share/tmp

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



