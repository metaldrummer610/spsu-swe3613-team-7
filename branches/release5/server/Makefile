CC=g++
EXENAME=server
CFLAGS=-Wall -I./include/ -I../common/include/ -o $(EXENAME) -std=gnu++0x
LDFLAGS=-lICDNetwork -L../common/bin/ -lenet -lpqxx -lpq /usr/local/lib/libboost_serialization.a
SRC=src/main.cpp

all : $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS)
	mv $(EXENAME) bin/

clean:
	rm -f bin/$(EXENAME)

run:
	./bin/server
