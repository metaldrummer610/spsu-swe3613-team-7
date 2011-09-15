CC=g++
EXENAME=server
CFLAGS=-Wall -I./include/ -I../common/include/ -o $(EXENAME)
LDFLAGS=/usr/local/lib/libenet.a -lICDNetwork -L../common/bin/
SRC=src/main.cpp

all : $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS)
	mv $(EXENAME) bin/

clean:
	rm -f bin/$(EXENAME)
