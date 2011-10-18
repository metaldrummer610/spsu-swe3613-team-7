CC=g++
CFLAGS=-Wall -I./include/ -c -std=gnu++0x
#SRC=src/ICD9.cpp src/ICD10.cpp src/CodeList.cpp src/BaseCode.cpp src/ICDPacket.cpp \
#	src/ICDCommandPacket.cpp src/ICDResponsePacket.cpp
SRC=src/*.cpp
OBJ=*.o
LIBNAME=libICDNetwork.a
DOXYFILE=Doxyfile

all : $(SRC)
	$(CC) $(CFLAGS) $(SRC)
	ar rvs $(LIBNAME) $(OBJ)
	mv $(LIBNAME) bin/
#	$(MAKE) test

test:
	cd tests && $(MAKE)

doc:
	doxygen $(DOXYFILE)

clean:
	@rm -f bin/$(LIBNAME) $(OBJ)
	@rm -f -R doc/html/
#	@cd tests && $(MAKE) clean
