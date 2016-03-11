LIBS=-l boost_system -l boost_filesystem  -l boost_thread
SRC=$(wildcard *.cpp)
MID=$(patsubst %.cpp,%.o,$(SRC))
OBJ=$(patsubst %.cpp,%,$(SRC))

all:$(OBJ)

%:%.cpp
	g++  $< -o $@  $(LIBS) 

.PHONY:clean

clean:
	-rm -rf $(MID) $(OBJ)
