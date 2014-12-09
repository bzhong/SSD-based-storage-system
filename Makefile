TARGET = main
OBJ = common/op_structure.o disk/disk.o algorithm/replacement_algo.o input_generator/input_generator.o driver/main.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -std=c++0x -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
INC = -I./../common -I./../algorithm -I./../disk -I./../input_generator
LIB = -L./common -L./algorithm -L./disk -L./input_generator -L./driver

all: input_generator/tinyxml2.h
	cd common; make
	cd disk; make
	cd algorithm; make
	cd input_generator; make
	cd driver; make
	$(CC) $(LFLAGS) $(LIB) -o $(TARGET) $(OBJ) input_generator/tinyxml2.cpp

clean:
	cd common; make clean
	cd disk; make clean
	cd algorithm; make clean
	cd input_generator; make clean
	cd driver; make clean
	rm $(TARGET)
