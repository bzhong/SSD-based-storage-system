CC = g++
DEBUG = -g
CFLAGS = -Wall -std=c++0x -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
INC = -I./../common -I./../disk

replacement_algo.o: replacement_algo.h ../disk/disk.h ../common/op_structure.h
	$(CC) $(INC) $(CFLAGS) replacement_algo.cpp
clean:
	rm  *.o
