CC = cc
CFLAGS = -O2 -Wall -pedantic -I/usr/local/include
LIBS = -lmaxminddb
LIBPATH= -L/usr/local/lib/
OBJS = geoiplookup2.o
BINARIES = geoiplookup2

all: clean ${BINARIES}

geoiplookup2: geoiplookup2.o
	$(CC) $(CFLAGS) $(OBJS) -o geoiplookup2 ${LIBPATH} $(LIBS)

clean:
	rm -f ${BINARIES}
	rm -f *.o
install:
	cp geoiplookup2 /usr/local/bin
