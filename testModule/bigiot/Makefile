CC=gcc
TARGET=bigiot-test
CFLAGS=-Wall 
OBJSRC= ../tcp/tcp.o	\
		 bigiot.o		\
		 bigiot-test.o

all:$(OBJSRC)
	$(CC) -o $(TARGET) $(OBJSRC) -pthread

$(filter %.o,$(OBJSRC)):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean

clean:
	rm $(TARGET) $(OBJSRC)



