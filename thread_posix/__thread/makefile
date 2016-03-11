
all:
	g++ -g  __thread.cc Tls.cc -o __thread -lpthread -Wall
	g++ -g  perthread.cc  -o perthread -lpthread -Wall
	gcc memcheck.c -o memcheck
	gcc __thread2.c -o __thread2 -lpthread

clean:
	-rm -rf __thread __thread2 memcheck

.PHONY:clean
