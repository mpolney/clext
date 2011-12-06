CFLAGS=-O4
INCLUDES=types.h mem.h io.h bmp.h unpack.h
OBJS=clext.o mem.o io.o bmp.o unpack.o

clext: $(OBJS) $(INCLUDES) Makefile
	cc $(OBJS) -o clext -Wall -pedantic $(CFLAGS)

.PHONY: clean

clean:
	rm -f clext $(OBJS)
