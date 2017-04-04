/* FOURBYFOUR */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h> /* for getrusage */
#include <unistd.h>
#include <assert.h>
#include <string.h>

#define ROW 4
#define COL 5

void generate_matrix(double *);
void reduce_matrix(double *restrict, double *restrict);
void error_check(double *restrict, double *restrict);
void print_results(struct rusage *stats);
void print_result_values(double *);
void _printm(double *restrict);


int
main(void)
{
    srand48(getpid());
    /* allocate heap memory for matrix */
    double *orig= malloc(sizeof(double) * ROW * COL);
    double *matrix = malloc(sizeof(double) * ROW * COL);
    double *results = malloc(sizeof(double) * ROW);


    /** generate and print starting matrix */
    generate_matrix(matrix);
    _printm(matrix);
    memcpy(orig, matrix, sizeof(double) * ROW * COL);
    _printm(orig);

    /* forward elimination */
    reduce_matrix(matrix, results);

    error_check(orig, results);

    /* print resulting matric and  get and print usage stats */
    print_result_values(results);
    struct rusage stats;
    getrusage(RUSAGE_SELF, &stats);
    print_results(&stats);

    free(matrix);
    free(results);
    exit(EXIT_SUCCESS);
}

void
/* forard elimintion */
reduce_matrix(double *restrict pivot, double *restrict results)
{
    double *restrict start = pivot;

    int i, j, k;                       /* 24 byte  */
    double rat;                           /*  8 byte  */

    // const size_t COLS = COL + 1;         /*  8 byte  */
    const int ROWS = ROW - 1;         /*  8 byte  */

    int r = ROW - 1;                  /*  8 byte  */
    double *restrict prime = pivot + COL;/*  8 byte  */
                                   /* total: 72 byte  */

    /* forward elimination */
    // for each row ...
    for (k = 0; k < ROWS; ++k) {

        assert(prime <= start + COL * ROW);
        assert(pivot <= start + COL * ROW);

        // for each leftmost value below the pivot value ...
        for (i = 0; i < r; prime += COL, ++i) {
            printf("pivot strt: %.2lu\n", &pivot[k] - start);
            printf("prime strt: %.2lu\n", &prime[k] - start);
            printf("pivot[k]: %.2lf\n", pivot[k]);
            printf("prime[k]: %.2lf\n", prime[k]);
            rat = prime[k] / pivot[k];

            // obliterate that shit
            printf("pivot: ");
            for (j = k; j < COL; ++j)
                printf("%.2lu ", &pivot[j] - start);
            printf("\n");

            printf("prime: ");
            for (j = k; j < COL; ++j)
                printf("%.2lu ", &prime[j] - start);
            printf("\n");

            for (j = k; j < COL; ++j) {
                printf("rat: %lf\n", rat);
                prime[j] -= (rat * pivot[j]);
            }
            printf("\n");

        }
        _printm(start);

        /**  after all leftmost values for current column have been obliterated
         * , increment pivot, and do unto thy neighbor.
         */
        pivot += COL;
        --r;
        prime = pivot + COL;
    }

    // return prime to last index of matrix
#   ifndef NDEBUG
    printf("\n\n\tpivot loc: %lu\n", pivot - start);
    printf("\tprime loc: %lu\n", prime - start);
#   endif

    --prime;
    results[0] = prime[0] / prime[-1];

    prime -= COL;

    for (i = 1; i < ROW; prime -= COL, ++i) {
        results[i] = prime[0];
        for (j = -1, k = 0; k < i; --j, ++k)
            results[i] -= prime[j] * results[k];
        results[i] /= prime[j];
    }

}


void
error_check(double *restrict matrix, double *restrict res)
{
    int i;
    double sums[ROW] = { 0.0 };
    double *restrict sums_itr = sums;
    for (i = 0; i < ROW; ++i) {
        *sums_itr += matrix[i] * res[i];
        matrix += COL + 1;
        ++sums_itr;
    }
    for (i = 0; i < ROW; ++i) {
        printf("res: %lf\n", res[i]);
        printf("sums: %lf\n", sums[i]);
    }
}

void
generate_matrix(double *restrict mat)
{
    double *restrict lol = mat;
    size_t i, j;
    for (i = 0; i < ROW; lol += COL, ++i) {
        for (j  = 0; j < COL; ++j)
        lol[j] = drand48() * 2.0 - 1.0;
    }
}

void
print_result_values(double *res)
{
    int i = ROW;
    char var = 97;

    printf("\n\n");
    for (; --i; ++var)
        printf("\t%+-.3lf%c\n", res[i], var);
}

void
print_results(struct rusage *stats)
{
    printf(
    "user CPU time         : %ld.%ld\n"
    "system CPU time       : %ld.%ld\n"
    "max resdident set size: %ld\n"
    "minor page faults     : %ld\n"
    "major page faults     : %ld\n"
    , stats->ru_utime.tv_sec, stats->ru_utime.tv_usec
    , stats->ru_stime.tv_sec, stats->ru_stime.tv_usec
    , stats->ru_maxrss
    , stats->ru_minflt
    , stats->ru_majflt);
    /*
     * stats.ru_utime   : struct timeval
     *       ru_utime.tv_sec    : time_t(whole seconds of elapsed time)
     *       ru_utime.tv_usec   : long int number of microseconds
     * stats.ru_stime   : struct timeval
     *       ru_stime.tv_sec    : time_t(whole seconds of elapsed time)
     *       ru_stime.tv_usec   : long int number of microseconds
     * stats.maxrss     : maximum resident set size
     * stats.ru_minflt  : page reclaims(soft page faults)
     * stats.ru_majflt  : page faults(hard page faults)
     */
}
void
_printm(double *restrict mat)
{
    size_t i, j;
    double *restrict start = mat;
    printf("\t\n\nmat:\n");
    for (i = 0; i < ROW; ++i) {
        printf("\t");
        char var = 97;
        for (j = 0; j < COL - 1; ++j, ++var)
            printf("%+-.2lf%c ", mat[j], var);
        printf(" = %.2lf ", mat[j]);
        printf("%lu\n", &mat[j] - start);
        mat += COL;
    }
}
