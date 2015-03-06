#ifndef MEM_USAGE_H
#define MEM_USAGE_H

#include <stdint.h> /* for fixed-width integers */

int32_t parse_proc_line(char* line);
int32_t get_proc_info(int32_t* phys_cur, int32_t* phys_high);
void print_memory_usage(const char* const prog_name, const uint32_t name_len);

#endif /* MEM_USAGE_H */
