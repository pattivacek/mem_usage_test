all:
	gcc -c mem_usage.c
	gcc -o rand_test random_dealloc.c mem_usage.o -g
	gcc -o block_test block_dealloc.c mem_usage.o -g

clean:
	rm a.out rand_test block_test *.o
