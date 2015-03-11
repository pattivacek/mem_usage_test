all:
	gcc -c mem_usage.c
	gcc mem_test.c mem_usage.o -g

clean:
	rm a.out *.o
