#include "mem_usage.h"

#include <stdlib.h> /* for calloc, free */
#include <stdio.h> /* for printf */
#include <string.h> /* for memset */

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
    uint32_t i, buf_count;
    const uint32_t init_buffer_count = 100;
    const uint32_t data_buf_size = 4 * 1024 * 1024;
    BUF_HEADER* buf_next;
    BUF_HEADER* buf_prev;

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
    // Set up initial buffer. Use data_buf_cur as a shortcut to the data portion
    // of the buffer, which follows in memory directly after the buffer header.
    buf_hdr_cur = buf_list_start;
    buf_count = init_buffer_count;

    printf("Total allocated buffers: %u\n", buf_count);
    fflush(stdout);

    print_memory_usage("mem_test", 8);

    // ADDITIONAL ALLOCATION

    buf_hdr_cur = buf_hdr_cur->next_buf->next_buf->next_buf->next_buf->next_buf;

    buf_next = buf_hdr_cur->next_buf;
    buf_prev = buf_hdr_cur;

    printf("Allocating additional buffers.\n");
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
    buf_hdr_cur->next_buf = buf_next;
    buf_hdr_cur = buf_prev;

    printf("Total allocated buffers: %u\n", buf_count);
    fflush(stdout);

    print_memory_usage("mem_test", 8);

    // DEALLOCATE ADDITIONAL BUFFERS

    printf("Deallocating extra buffers.\n");
    fflush(stdout);

    buf_next = buf_hdr_cur->next_buf;

    // One by one, free the excess buffers.
    while (buf_count > init_buffer_count)
    {
        buf_next = buf_next->next_buf;

        free(buf_prev->next_buf);
        buf_prev->next_buf = buf_next;
        buf_count--;
    }

    printf("Total allocated buffers: %u\n", buf_count);
    fflush(stdout);

    print_memory_usage("mem_test", 8);

    // DEALLOCATE INITIAL BUFFERS

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