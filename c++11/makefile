LIBS=-l boost_system -l boost_filesystem  -l boost_thread
SRC=$(wildcard *.cc)
MID=$(patsubst %.cc,%.o,$(SRC))
OBJ=$(patsubst %.cc,%,$(SRC))

all:$(OBJ)

%:%.cc
	g++  -std=c++11 $< -o $@  $(LIBS) 

.PHONY:clean

clean:
	-rm -rf $(MID) $(OBJ)
