/** @file output.c print functions for gaussian elimination
 *  linear solver using Gaussian elimination with partial pivoting
 */

#include "gauss_types.h"
#include <stdio.h>

void
print_stats(struct _Stats *stats)
{
    printf(
    "\n"
    "\n\tcores:   %d\n"
    "\n\tthreads: %i\n"
    "\n\tTIME:    %lf seconds\n"
    "\n\tL2 norm: %.10le\n"
    , stats->cores
    , stats->threads
    , stats->time
    , stats->l2_norm
    );
}

void
print_values(
      double *const original
#   ifdef NDEBUG
    , double *const variables
#   endif
    , const int ROW
    , const int COL)
{
    int i, j = 0, col = 0;

    printf("\noriginal matrix:");
    for (i = 0; i < ROW; ++i) {

        col += COL
    #ifndef NDEBUG
        , printf("\n\t");
        while (j < col - 1)

            printf("%+-.2lf ", original[j++]);

        printf(" | %+-.2lf ", original[j++]);
    }
    #else
        , printf("\n");
        while (j < col - 1)

            printf("%+-.10le ", original[j++]);

        printf(" | %+-.10le ", original[j++]);
    }

    printf("\n\nx values:\n");

    for (i = 0; i < ROW; ++i)

        printf("x%d = %.10le\n", i + 1, variables[i]);
    #endif
}

#ifndef NDEBUG
void
print_matrix_debug(
      const char *name
    , double **matrix
    , double **const ROWS_LAST
    , const int ROW
    )
{
    printf("\t\n\n%s\n", name);
    while (matrix < ROWS_LAST) {

        printf("\t");

        int j;
        for (j = 0; j < ROW; ++j)

            printf("%+-.2lf ", matrix[0][j]);

        printf(" | %+-.2lf\n", matrix[0][j]);

        ++matrix;

    }
}
#endif

/*
static int
_total_cache_size()
{
#   define L1D_CACHE 0
    unsigned int eax = 4, ebx = 0, ecx = L1D_CACHE, edx = 0;
    __asm__("cpuid": "+a"(eax), "=b"(ebx), "+c"(ecx), "=d"(edx));
    return (int)(((ebx & 0x0fff) + 1) // line size
        * (((ebx >> 12) & 0x3ff) + 1) // line partitions
        * (((ebx >> 22) & 0x3ff) + 1) // ways of assoc
        * (ecx + 1));                 // cache sets
}
*/
