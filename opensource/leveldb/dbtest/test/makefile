LIB=$(HOME)/leveldb/leveldb/libleveldb.a -lpthread
INC=-I$(HOME)/leveldb/leveldb/include/ 
SRC=$(wildcard *.cpp)
DES=$(patsubst %.cpp, %,$(SRC))
CC=g++
DEBUG= -Wall -g
all:$(DES)
	@echo 'done!' 
$(DES):%:%.cpp
	$(CC) $< -o $@ $(LIB) $(INC) $(DEBUG)

.PHONY:clean

clean:
	-rm -rf $(DES)
	@echo 'clean done!'
