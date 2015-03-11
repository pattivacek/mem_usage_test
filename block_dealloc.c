#include "mem_usage.h"

#include <stdlib.h> /* for calloc, free, rand, rand */
#include <stdio.h> /* for printf */
#include <string.h> /* for memset */
#include <unistd.h> /* for sleep */

/* Linked list header for array data packets. */
typedef struct BUF_HEADER_
{
    struct BUF_HEADER_* next_buf;
    uint8_t ii_filled;
    uint8_t RESERVED[7];
} BUF_HEADER;

int main(int argc, char* argv[])
{
    BUF_HEADER* buf_list_start = NULL;
    BUF_HEADER* buf_hdr_cur = NULL;
    BUF_HEADER* buf_block_heads[100];
    BUF_HEADER* buf_block_ends[100];
    uint32_t i, buf_count, prev_blocks, num_blocks;
    const uint32_t init_buffer_count = 100;
    const uint32_t data_buf_size = 1 * 1024 * 1024;
    BUF_HEADER* buf_next;
    BUF_HEADER* buf_prev;
    int action;

    // INITIALIZATION

    printf("Start.\n");
    fflush(stdout);

    print_memory_usage("mem_test", 8);

    // Allocate an array of buffers, which we will form into a circular linked
    // list.
    buf_list_start = (BUF_HEADER*)calloc(1, data_buf_size);
    if (buf_list_start == NULL)
    {
        printf("ERROR: Failed to allocate memory for buf_list_start. "
               "Exiting.\n");
        return 1;
    }
    memset(buf_list_start, 0, data_buf_size);
    buf_hdr_cur = buf_list_start;
    for (i = 1; i < init_buffer_count; i++)
    {
        buf_hdr_cur->next_buf = (BUF_HEADER*)calloc(1, data_buf_size);
        if (buf_hdr_cur->next_buf == NULL)
        {
            printf("ERROR: Failed to allocate memory for packet buffer. "
                   "Exiting.\n");
            return 1;
        }
        buf_hdr_cur = buf_hdr_cur->next_buf;
        memset(buf_hdr_cur, 0, data_buf_size);
    }
    buf_hdr_cur->next_buf = buf_list_start;
    buf_count = init_buffer_count;
    // Leave buf_hdr_cur pointing at last allocated buffer.
    num_blocks = 1;
    buf_block_heads[num_blocks - 1] = buf_list_start;
    buf_block_ends[num_blocks - 1] = buf_hdr_cur;

    printf("Total allocated buffers: %u\n", buf_count);
    fflush(stdout);

    print_memory_usage("mem_test", 8);

    while (1)
    {
        sleep(1);
        // "Randomly" decide whether to allocate a block of 100 buffers or
        // deallocate half of all buffer blocks. Enforce a lower limit of one
        // full block to prevent segfaults and keep the test interesting.
        action = 1 + rand() % 2;

        if (num_blocks == 1 || action == 1)
        {
            // ADDITIONAL ALLOCATION

            buf_next = buf_hdr_cur->next_buf;
            buf_prev = buf_hdr_cur;

            printf("Allocating an additional block of buffers.\n");
            fflush(stdout);

            // Allocate a batch of new buffers and fit them into
            // the existing linked list.
            for (i = 0; i < init_buffer_count; i++)
            {
                buf_hdr_cur->next_buf = (BUF_HEADER*)calloc(1, data_buf_size);
                if (buf_hdr_cur->next_buf == NULL)
                {
                    // If we run out of memory and can't
                    // allocate additional buffers, we are in
                    // trouble. We may drop data.
                    printf("ERROR: Failed to allocate memory for packet buffer. "
                           "Attempting to continue anyway.\n");
                    fflush(stdout);
                    break;
                }
                buf_count++;
                buf_hdr_cur = buf_hdr_cur->next_buf;
                memset(buf_hdr_cur, 0, data_buf_size);
            }
            // Leave buf_hdr_cur pointing at last allcoated buffer, and store
            // the pointer to the first buffer we just allocated as the head of
            // this block.
            buf_hdr_cur->next_buf = buf_next;
            num_blocks++;
            buf_block_heads[num_blocks - 1] = buf_prev->next_buf;
            buf_block_ends[num_blocks - 1] = buf_hdr_cur;

            printf("Total allocated buffers: %u\n", buf_count);
            fflush(stdout);

            print_memory_usage("mem_test", 8);
        }
        else
        {
            // DEALLOCATE ADDITIONAL BUFFERS

            printf("Deallocating some blocks of buffers.\n");
            fflush(stdout);

            prev_blocks = num_blocks;

            while (num_blocks > prev_blocks / 2)
            {
                buf_next = buf_block_heads[num_blocks - 1];
                buf_prev = buf_block_ends[num_blocks - 2];

                // Make sure the end of the second-to-last block is properly
                // linked to the start of the last block.
                if (buf_prev->next_buf != buf_next)
                {
                    printf("Warning: prev->next != next!\n");
                }

                // One by one, free the excess buffers.
                while (buf_next != buf_list_start)
                {
                    buf_next = buf_next->next_buf;
                    free(buf_prev->next_buf);
                    buf_prev->next_buf = buf_next;
                    buf_count--;
                }

                // Make sure the end of the last block is properly linked to the
                // start of the first block.
                if (buf_next != buf_list_start)
                {
                    printf("Warning: next != start!\n");
                }

                num_blocks--;
            }
            // Reset buf_hdr_cur in case we freed it. Keep it at the end of the
            // last block.
            buf_hdr_cur = buf_block_ends[num_blocks - 1];

            printf("Total allocated buffers: %u\n", buf_count);
            fflush(stdout);

            print_memory_usage("mem_test", 8);
        }
    }

    // DEALLOCATE REMAINING BUFFERS

    printf("Deallocating initial buffers.\n");
    fflush(stdout);

    // Skip the first buffer, but set it's next_buf to NULL so that we'll know
    // when we've gotten back to the start.
    buf_hdr_cur = buf_list_start->next_buf;
    buf_list_start->next_buf = NULL;

    while (buf_hdr_cur != NULL)
    {
        BUF_HEADER* prev_buf = (BUF_HEADER*)buf_hdr_cur;
        buf_hdr_cur = buf_hdr_cur->next_buf;
        free(prev_buf);
    }

    printf("Total allocated buffers: %u\n", buf_count);
    fflush(stdout);

    print_memory_usage("mem_test", 8);


}
