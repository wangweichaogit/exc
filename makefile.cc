LIBS=-lpthread 
SRC=$(wildcard *.c)
MID=$(patsubst %.c,%.o,$(SRC))
OBJ=$(patsubst %.c,%,$(SRC))

all:$(OBJ)

$(OBJ):%:%.c
	g++  $< -o $@  $(LIBS) 

.PHONY:clean

clean:
	-rm -rf $(MID) $(OBJ)
