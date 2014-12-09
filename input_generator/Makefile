CC = g++
DEBUG = -g
CFLAGS = -Wall -std=c++0x -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
INC = -I./../common -I./../algorithm -I./../disk

input_generator.o: input_generator.h tinyxml2.h ../algorithm/replacement_algo.h ../common/op_structure.h
	$(CC) $(INC) $(CFLAGS) input_generator.cpp
clean:
	rm  *.o
