#g++ -arch i386 -arch x86_64 -I/usr/local/include -I/usr/local/include/FL/images -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT -o 'main' 'src/main.cpp' /usr/local/lib/libfltk.a -lpthread -framework Cocoa

CC=g++
EXENAME=client
#CFLAGS=-Wall -Iinclude/ -I../../common/include/ -o $(EXENAME)  -arch x86_64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT
#LDFLAGS=/usr/local/lib/libenet.a -lICDNetwork -L../../common/bin/ -lfltk -framework Cocoa -lpthread

CFLAGS=-Wall -Iinclude/ -I../../common/include/ -o $(EXENAME) -g
LDFLAGS=/usr/local/lib/libenet.a -lICDNetwork -L../../common/bin/ -L/usr/local/lib -lfltk -lXext -lXft -lfontconfig -lpthread -ldl -lm -lX11
SRC=src/main.cpp

all : $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS)
	mv $(EXENAME) bin/

clean:
	rm -f bin/$(EXENAME)

run:
	./bin/$(EXENAME)
