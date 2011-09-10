CC=g++
CFLAGS=-Wall -g -o networkTest
INCLUDES=-I../include/
LIBS=-lcppunit -lICDNetwork -L../bin/
SRCS=src/testBaseCode.cpp

all:
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) $(INCLUDES)

clean:
	rm -f networkTest
