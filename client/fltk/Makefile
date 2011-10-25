CC=g++
EXENAME=client
CFLAGS=-Wall -Iinclude/ -I../../common/include/ -o $(EXENAME) -g -std=gnu++0x $(shell fltk-config --cxxflags)
LDFLAGS=/usr/local/lib/libenet.a -lICDNetwork -L../../common/bin/ /usr/local/lib/libboost_serialization.a $(shell fltk-config --ldflags)
SRC=src/main.cpp #src/help.cpp

all : $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS)
	mv $(EXENAME) bin/

clean:
	rm -f bin/$(EXENAME)

run:
	./bin/$(EXENAME)
