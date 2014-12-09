CC = g++
DEBUG = -g
CFLAGS = -Wall -std=c++0x -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
INC = -I./../common -I./../algorithm -I./../disk -I./../input_generator

main.o: ../input_generator/input_generator.h ../algorithm/replacement_algo.h ../disk/disk.h
	$(CC) $(INC) $(CFLAGS) main.cpp
clean:
	rm  *.o
