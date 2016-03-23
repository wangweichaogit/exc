SRC=$(wildcard *.c)
MID=$(patsubst %.c,%.o,$(SRC))
OBJ=$(patsubst %.c,%,$(SRC))

all:$(OBJ)

$(OBJ):%:%.c
	gcc  $< -o $@   

.PHONY:clean

clean:
	-rm -rf $(MID) $(OBJ)
