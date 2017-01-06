all:
	gcc -c mem_usage.c
	gcc -o free_every_other free_every_other.c mem_usage.o -g
	gcc -o free_by_block free_by_block.c mem_usage.o -g

clean:
	rm -f free_every_other free_by_block *.o
