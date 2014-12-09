CC = g++
DEBUG = -g
CFLAGS = -Wall -std=c++0x -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
INC = -I./../common

disk.o: disk.h ../common/op_structure.h
	$(CC) $(INC) $(CFLAGS) disk.cpp
clean:
	rm  *.o
