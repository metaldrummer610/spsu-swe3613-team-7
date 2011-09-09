CC=g++
CFLAGS=-Wall -I./include/ -c
LDFLAGS=
SRC=src/ICD9.cpp src/ICD10.cpp src/CodeList.cpp
OBJ=*.o
LIBNAME=libICDNetwork.a

all : $(SRC)
	$(CC) $(CFLAGS) $(SRC)
	ar rvs $(LIBNAME) $(OBJ)
	mv $(LIBNAME) bin/

clean:
	rm -f bin/$(LIBNAME) $(OBJ)
