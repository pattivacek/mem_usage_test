all:
	gcc -c mem_usage.c
	gcc mem_test.c mem_usage.o

clean:
	rm a.out *.o
