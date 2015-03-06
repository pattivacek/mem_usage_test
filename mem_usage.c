/* This file offers a simple call to print out the memory usage (RAM, not
 * virtual) of the system and the current process. */

#ifdef  __linux__
#   define _GNU_SOURCE /* for strnlen */
#endif

#include "mem_usage.h"
#ifdef _WIN32
#   include <windows.h> /* for GlobalMemoryStatusEx and MEMORYSTATUSEX */
#   include <psapi.h> /* for GetProcessMemoryInfo and PROCESS_MEMORY_COUNTERS_EX */
#elif defined  __linux__
#   include <sys/sysinfo.h> /* for sysinfo */
#endif
#include <stdio.h> /* for fprint, fopen, fgets */
#include <string.h> /* for strnlen (with _GNU_SOURCE) */
#include <stdlib.h> /* for atoi */

/* The basic ideas here were borrowed from here:
   http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
*/

#ifdef  __linux__

#define LINE_LENGTH 128

int32_t parse_proc_line(char* line)
{
    int32_t i;
    i = (int32_t)strnlen(line, LINE_LENGTH);
    // Remove unit specifier text ('kB').
    line[i-3] = '\0';
    // Move to numeric value portion.
    while (*line < '0' || *line > '9') line++;
    // Convert numeric value.
    i = atoi(line);
    return i;
}

// Note: all values are returned in kB.
int32_t get_proc_info(int32_t* phys_cur, int32_t* phys_high)
{
    FILE* file = fopen("/proc/self/status", "r");
    int32_t result = 2;
    char line[LINE_LENGTH];

    while (fgets(line, LINE_LENGTH, file) != NULL)
    {
        if (strncmp(line, "VmRSS:", 6) == 0)
        {
            *phys_cur = parse_proc_line(line);
            result--;
        }
        else if (strncmp(line, "VmHWM:", 6) == 0)
        {
            *phys_high = parse_proc_line(line);
            result--;
        }
        if (!result)
        {
            // Found all desired values.
            break;
        }
    }
    fclose(file);
    return result;
}

#endif /* #ifdef  __linux__ */

void print_memory_usage(const char* const prog_name, const uint32_t name_len)
{
#ifdef _WIN32

    MEMORYSTATUSEX memInfo;
    PROCESS_MEMORY_COUNTERS_EX pmc;
    DWORDLONG phys_total, phys_used;
    SIZE_T phys_cur, phys_high;
    uint32_t phys_total_k, phys_used_k, phys_cur_k, phys_high_k;

    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    phys_total = memInfo.ullTotalPhys;
    phys_used = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

    GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
    phys_cur = pmc.WorkingSetSize;
    phys_high = pmc.PeakWorkingSetSize;

    phys_total_k = (uint32_t)(phys_total / 1024);
    phys_used_k  = (uint32_t)(phys_used  / 1024);
    phys_cur_k   = (uint32_t)(phys_cur   / 1024);
    phys_high_k  = (uint32_t)(phys_high  / 1024);

    printf("%.*s: Total physical memory: %u kB; currently used: %u kB\n",
           name_len, prog_name, phys_total_k, phys_used_k);
    printf("%.*s: Physical memory used by current process: %u kB; peak use: %u kB\n",
           name_len, prog_name, phys_cur_k, phys_high_k);
    fflush(stdout);

#elif defined  __linux__

    struct sysinfo memInfo;
    int32_t result, phys_cur = 0, phys_high = 0;
    uint64_t phys_total, phys_used;
    uint32_t phys_total_k, phys_used_k;

    result = get_proc_info(&phys_cur, &phys_high);
    if (result != 0)
    {
        printf("%.*s: Warning: Did not read all values from /proc/self/status as expected.\n",
               name_len, prog_name);
    }

    sysinfo(&memInfo);

    phys_total = memInfo.totalram;
    phys_total *= memInfo.mem_unit;
    phys_total_k = phys_total / 1024;

    phys_used = memInfo.totalram - memInfo.freeram;
    phys_used *= memInfo.mem_unit;
    phys_used_k = phys_used / 1024;

    printf("%.*s: Total physical memory: %u kB; currently used: %u kB\n",
           name_len, prog_name, phys_total_k, phys_used_k);
    printf("%.*s: Physical memory used by current process: %d kB; peak use: %d kB\n",
           name_len, prog_name, phys_cur, phys_high);
    // This flush is necessary for KAEL. I guess the Fortran flush doesn't flush
    // the output buffers in the C code. Whatever.
    fflush(stdout);

#endif

    return;
}

