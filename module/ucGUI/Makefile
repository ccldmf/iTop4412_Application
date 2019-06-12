
CC=arm-none-linux-gnueabi-ar

SUBDIRS=AntiAlias ConvertMono Font Camera LCDDriver MemDev Widget ConvertColor Core JPEG MultiLayer WM TouchScreen

LIBOBJS=
LIBOBJS+=$(wildcard AntiAlias/*.o)
LIBOBJS+=$(wildcard ConvertMono/*.o)
LIBOBJS+=$(wildcard Font/*.o)
LIBOBJS+=$(wildcard LCDDriver/*.o)
LIBOBJS+=$(wildcard MemDev/*.o)
LIBOBJS+=$(wildcard Widget/*.o)
LIBOBJS+=$(wildcard ConvertColor/*.o)
LIBOBJS+=$(wildcard Core/*.o)
LIBOBJS+=$(wildcard JPEG/*.o)
LIBOBJS+=$(wildcard MultiLayer/*.o)
LIBOBJS+=$(wildcard WM/*.o)
LIBOBJS+=$(wildcard TouchScreen/*.o)
LIBOBJS+=$(wildcard Camera/*.o)


all:
	for name in $(SUBDIRS);	\
		do					\
			cd $$name;		\
			make;			\
			cd ../;			\
		done

	make guilib

.PHONY:guilib
guilib:
	$(CC) rv libucgui.a $(LIBOBJS)
	cp libucgui.a /home/cc/share/tmp
	@echo "Finished Create libucgui.a!!!"



.PHONY:clean
clean:
	rm -f libucgui.a
	for name in $(SUBDIRS);	\
		do					\
			cd $$name;		\
			make clean;		\
			cd ../;			\
		done
	@echo "Finish clean!!!"




