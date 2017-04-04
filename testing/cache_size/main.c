/** Written by Cy Baca with much help from
 * http://stackoverflow.com/questions/21369381/measuring-cache-latencies
 * and http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-manual-325462.html
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* sysconf */
#include <stdint.h>
#include <assert.h>
#include <string.h>

enum {
      LD1 = 0x0001
    , LI1 = 0x0002
    , L2 = 0x0004
    , L3 = 0x0008
    , CACHE_LINE_SIZE = 0x0010
    , CACHE_LINE_PART = 0x0020
    , CACHE_WAYS_OF_ASSOC = 0x0040
    , CACHE_TOTAL_SIZE = 0x0080
    , SETS = 0x0100
    , CACHE_TYPE = 0x001f
};

struct {
    unsigned int LD1 : 1;
    unsigned int LI1 : 1;
    unsigned int L2 : 1;
    unsigned int L3 : 1;

    unsigned int CACHE_LINE_SIZE : 1;
    unsigned int CACHE_LINE_PART : 1;
    unsigned int CACHE_WAYS_OF_ASSOC : 1;
    unsigned int CACHE_TOTAL_SIZE: 1;
    unsigned int SETS: 1;
} _flags;

void print_data(int *);
void i386_cpuid_caches();
void usage();

int
main(int argc, char **argv)
{
    if (argc < 2) {
    }


    switch (argc) {
        case 3:
            if (strcmp(argv[2], "-ld") == 0)
                _flags.LD1 = 1;
            else if (strcmp(argv[2], "-li") == 0)
                _flags.LI1 = 1;
            else if (strcmp(argv[2], "-l2") == 0)
                _flags.L2 = 1;
            else if (strcmp(argv[2], "-l3") == 0)
                _flags.L3 = 1;
        case 2:
            if (strcmp(argv[1], "-line_size") == 0)
                _flags.CACHE_LINE_SIZE = 1;
            else if (strcmp(argv[1], "-line_partitions") == 0)
                _flags.CACHE_LINE_PART = 1;
            else if (strcmp(argv[1], "-ways") == 0)
                _flags.CACHE_WAYS_OF_ASSOC = 1;
            else if (strcmp(argv[1], "-total_size") == 0)
                _flags.CACHE_TOTAL_SIZE = 1;
            else if (strcmp(argv[1], "-sets") == 0)
                _flags.SETS = 1;
            else if (strcmp(argv[1], "--info") == 0) {
                i386_cpuid_caches();
                exit(EXIT_SUCCESS);
            }
            else
                printf("SAY WHAT?\n");
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
    }

    print_data((int *)&_flags);
    exit(EXIT_SUCCESS);
}

void
print_data(int *state)
{
    /* eax: accumulator for operands and results data
     * ebx: pointer do data in the DS segment
     * ecx: counter for string and loop operations
     * edx: I/O pointer
     *
     * esi: pointer to data in the segment pointed to by the DS register
     * edi: pointer to data(or destination) in the segment pointed to by
     *      the ES register; destination pointer for string operations
     * esp: stack pointer(in the SS segment)
     * ebp: pointer to data on the stack(in the SS segment)
     */
    // variables to hold contents of 4 i386 legacy registers
    uint32_t eax = 4 // 4 == cache info
           , ebx = 0
           , ecx = 0 // cache id
           , edx = 0;
    if (state[0] & LD1) {
    //     printf("Level 1 Data Cache:\n");
        ecx = 0;
    } else if (state[0] & LI1) {
    //     printf("Level 1 Instruction Cache\n");
        ecx = 1;
    } else if (state[0] & L2) {
    //     printf("Level 2 Cache:\n");
        ecx = 2;
    } else if (state[0] & L3) {
    //     printf("Level 3 Cache:\n");
        ecx = 3;
    } else {
    //     printf("Level 1 Data Cache:\n");
        ecx = 0;
    }

    // this will generate output in the 4 registers
    __asm__(
        "cpuid"
        : "+a" (eax) // holds cpuid command code, 4 means cache query
        , "=b" (ebx)
        , "+c" (ecx) // cache id
        , "=d" (edx)
    );

    if (state[0] & CACHE_LINE_SIZE) {
        printf("%d\n", (ebx & 0xFFF) + 1);
    } else if (state[0] & CACHE_LINE_PART) {
        printf("%d\n", ((ebx >> 12) & 0x3FF) + 1);
    } else if (state[0] & CACHE_WAYS_OF_ASSOC) {
        printf("%d\n", ((ebx >> 22) & 0x3FF) + 1);
    } else if (state[0] & CACHE_TOTAL_SIZE) {
        printf("%u\n"
                    , ((ebx & 0x0fff) + 1)
                    * (((ebx >> 12) & 0x3ff) + 1)
                    * (((ebx >> 22) & 0x3ff) + 1)
                    * (ecx + 1));
    } else if (state[0] & SETS) {
        printf("%d\n", ecx + 1);
    }
}

void
i386_cpuid_caches()
{
    printf("CPU info for this pc:\n");
    uint32_t i;
    for (i = 0; i < 32; i++) {
        uint32_t eax = 4; /* for cache info */
        uint32_t ebx; /* cache id */
        uint32_t ecx = i;
        uint32_t edx;
        __asm__( "cpuid": "+a" (eax), "=b" (ebx), "+c" (ecx), "=d" (edx) );
        int cache_type = eax & CACHE_TYPE;
        if (cache_type == 0)
            break;
        char cache_type_string[64];
        switch (cache_type) {
            case 1:
                strcpy(&cache_type_string[0], "Data Cache");
                break;
            case 2:
                strcpy(&cache_type_string[0], "Instruction Cache");
                break;
            case 3:
                strcpy(&cache_type_string[0], "Unified Cache");
                break;
            default:
                strcpy(&cache_type_string[0], "Unknown Type Cache");
                break;
        }
        int cache_level = (eax >>= 5) & 0x7;
        int cache_is_self_initializing = (eax >>= 3) & 0x1;
        int cache_is_fully_associative = (eax >>= 1) & 0x1;

        unsigned int cache_coherency_line_size = (ebx & 0xFFF) + 1;
        unsigned int cache_physical_line_partitions =
        ((ebx >>= 12) & 0x3FF) + 1;
        unsigned int cache_ways_of_associativity = ((ebx >>= 10) & 0x3FF) + 1;

        unsigned int cache_sets = ecx + 1;

        size_t cache_total_size = cache_ways_of_associativity
                                * cache_physical_line_partitions
                                * cache_coherency_line_size
                                * cache_sets;
        printf(
            "Cache ID %u:\n"
            "- Level: %d:\n"
            "- Type: %s\n"
            "- Sets: %d\n"
            "- System Coherency Line Size: %d bytes\n"
            "- Physical Line partitions: %d\n"
            "- Ways of associativity: %d\n"
            "- Total Size: %zu bytes(%zu kb)\n"
            "- Is fully associative: %s\n"
            "- Is Self Initializing: %s\n"
            "\n", i, cache_level, cache_type_string, cache_sets,
            cache_coherency_line_size, cache_physical_line_partitions,
            cache_ways_of_associativity, cache_total_size,
            cache_total_size >> 10,
            cache_is_fully_associative ? "true" : "false",
            cache_is_self_initializing ? "true" : "false"
        );
    }
}

void
usage()
{
    fprintf(stderr,
"cache_info --<opt> -[cache level]\n"
"opt:\n"
"\t--sets:\n"
"\t--line_size:         System Coherency Line Size.\n"
"\t--line_partitions:   Pysical Line paritions.\n"
"\t--ways:              Ways of associativity.\n"
"\t--total_size:        Total size of cache.\n"
    );
}
