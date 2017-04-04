/** @file public_gauss.c
 *  implementation of public methods for
 *  linear solver using Gaussian elimination with partial pivoting
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for getpid(), sysconf() */
#include <assert.h>
#include <math.h>
#include <omp.h>
#include "gauss_internal.h"

void
gaussian_init(const int N)
{
    /* instantiate global vars */
    _COL = N + 1;
    _ROW = N;
    _CLS = (unsigned)sysconf(_SC_LEVEL1_DCACHE_LINESIZE);

    /* allocate vectors on heap and instatiate with random numbers */
    if (!vectors_init(&_vectors, (size_t)N, _CLS)) {

        fprintf(stderr, "Failed to initialize vector data\n");

        exit(EXIT_FAILURE);

    }

    /* omp info for end of program */
    #pragma omp parallel default(none) shared(_stats)

    #pragma omp master

        _stats.threads = omp_get_num_threads(),

        _stats.cores = omp_get_num_procs();

    _ready = 1;
}

void
gaussian_execute()
{
    if (!_ready) {

        fprintf(stderr, "GAUSSIAN ELIMINATION NOT INITIALIZED!!\n");

        exit(EXIT_FAILURE);

    }
        print_values(
          _vectors.original
#       ifdef NDEBUG
        , _vectors.variables
#       endif
        , _ROW
        , _COL);


    /* start time */
    _stats.time = omp_get_wtime();

    /* reduce matrix + partial pivoting */
    echelon_reduce(_vectors.rows, _ROW, _COL);

    /* backward substitution on upper triangle */
    back_substitution(
          _vectors.rows, _vectors.ROWS_LAST, _vectors.VARIABLES_LAST, _ROW);

    /* get time */
    _stats.time = omp_get_wtime() - _stats.time;

    /* get l2_norm */
    _stats.l2_norm = l2_norm_validity_check(
          _vectors.original, _vectors.variables, _ROW, _COL);
}

/** public function to call when execution is done. Deallocates any heap
 *  memory used during execution, and prints out runtime data.
 */
void
gaussian_finalize(void)
{

    if (_ROW < EPSILON)

        print_values(
          _vectors.original
#       ifdef NDEBUG
        , _vectors.variables
#       endif
        , _ROW
        , _COL);

    print_stats(&_stats);

    vectors_free(&_vectors);
}
