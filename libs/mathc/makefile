CC=gcc
AR=ar rcs
RANLIB=ranlib
LIB=libmathc.a

CFLAGS=-Wall -std=c99 -O3

$(LIB): mathc.o
	$(AR) $(LIB) mathc.o
	$(RANLIB) $(LIB)

mathc.o:
	$(CC) $(CFLAGS) -c mathc.c -o mathc.o

clean:
	rm -f mathc.o
	rm -f libmathc.a
