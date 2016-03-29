LIB=/usr/lib/x86_64-linux-gnu/libdb_cxx.a -lpthread
INC= -I/usr/include/
#-I$(HOME)/leveldb/leveldb/include/  -I/usr/include/
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
