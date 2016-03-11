src=$(wildcard *.c)
des=$(patsubst %.c,%,$(src))

all:$(des)
	@echo "make done !"
	
$(des):%:%.c
	gcc $< -o $@ -lpthread

.PHONY:clean

clean:
	-rm -rf $(des)
	@echo "clean done !"
