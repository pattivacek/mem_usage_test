# mem_usage_test
A demonstration of OS memory management.

This code creates two executables that do similar but slightly different things. Both programs operate in a loop and "randomly" decide to allocate or deallocate large chunks of memory. The programs begin with a 1 MB block and use a linked list to keep track of the allocated blocks. If they decide to allocate, they allocate 100 blocks, each 1 MB. If the free_by_block program decides to deallocate, it frees half of the buffers by traversing the linked list or deallocating sequentially. If free_every_other decides to deallocate, it frees half of the buffers by traversing the linked list and deallocating every other block. Both programs display OS and program memory statistics after each step.

The point is to show that memory is managed by most OSes in pages, and if part of a page's memory is allocated by a program, the OS will not reassign the page until the program has deallocated all the memory it held in the page. The free_every_other program will hold on to every page it touches over time, meaning it builds up a larger memory footprint. The free_by_block program will not have this problem.

This can be an important consideration for an application that needs to allocate additional memory to handle bursts from an upstream data source or blocking by a downstream data consumer and then later free the excess memory.

The block sizes and block allocation count can be (programmatically) reconfigured to amplify (or reduce) the problem.
