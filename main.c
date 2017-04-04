/* linear solver using Gaussian elimination with partial pivoting */
#include "gauss.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef NDEBUG
#   define N 6
#else
#   define N 4
#endif

#define THIS_MUCH 1
#define THAT_MUCH 9000
int get_l1d_cache_size(void);

int
main(int argc, char **argv)
/** PROGRAM START **/
{
    int n = N;

    if (argc > 1) {

        if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0) {

            __builtin_printf(
            "\n\tl1d cache size: %i bytes\n\n"
            , get_l1d_cache_size()
            );

            exit(EXIT_SUCCESS);

        }

        n = atoi(argv[1]);

    }

    if (n < THIS_MUCH)

        fprintf(stderr, "0 and below is too small\n")

        , exit(EXIT_FAILURE);


    else if (n > THAT_MUCH)

        fprintf(stderr, "\n\nITS OVER 9000!!!!\n"
"                            ,\n                      ,   ,'|\n               "
"     ,/|.-'   \\.\n                 .-'  '       |.\n           ,  .-'       "
"      |\n          /|,'                 |'\n         / '                    |"
",\n        /                       ,'/\n     .  |          _              /\n"
"      \\`' .-.    ,' `.           |\n       \\ /   \\ /      \\          /\n "
"       \\|    V        |        |  ,\n         (           ) /.--.   ''\"/\n "
"        \"b.`. ,' _.ee'' 6)|   ,-'\n           \\\"= --\"\"  )   ' /.-'\n    "
"        \\ / `---\"   .\"|'\n             \\\"..-    .'  |.\n               `"
"-__..-','   |\n             __.) ' .-'/    /\\._\n       _.--'/----..--------"
". _.-\"\"-._\n    .-'_)   \\.   /     __..-'     _.-'--.\n   / -'/      \"\""
"\"\"\"\"\"\"\"  ""       ,'-.   . `.\n  | ' /                        /    `  "
"`.  \\ \n  |   |                       |           | |\n   \\ .'\\           "
"           |     \\       |\n  / '  | ,'               . -  \\`.    |  / /\n "
"/ /   | |                      `/\"--. -' /\\ \n| |     \\ \\                "
"     /     \\     | \n| \\      | \\                  .-|      |    | \n     "
)
        , exit(EXIT_FAILURE);

    // gaussian elimination
    gaussian_init(n);
    gaussian_execute();
    gaussian_finalize();

    exit(EXIT_SUCCESS);
}

int
get_l1d_cache_size(void)
#   define L1D_CACHE 0
{
    unsigned int eax = 4, ebx = 0, ecx = L1D_CACHE, edx = 0;
    __asm__("cpuid": "+a"(eax), "=b"(ebx), "+c"(ecx), "=d"(edx));
    return (int)(((ebx & 0x0fff) + 1) // line size
        * (((ebx >> 12) & 0x3ff) + 1) // line partitions
        * (((ebx >> 22) & 0x3ff) + 1) // ways of assoc
        * (ecx + 1));                 // cache sets
}
