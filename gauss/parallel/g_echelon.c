/** @file echelon.c
 *  implimation of functions needed to create the upper right aka echelon
 *  matrix needed for backwards substitution for
 *  linear solver using Gaussian elimination with partial pivoting
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>


static void
_echelon_annihilate_next_row(
      double *restrict d_vec
    , double *restrict p_vec
    , const double COEF
    , const int CHUNK_LEN)
{
    const double *const DVEC_END = d_vec + CHUNK_LEN;
    while (d_vec < DVEC_END)

        *d_vec++ -= COEF * *p_vec++;

}

static void
_echelon_annihilation(
      double **const restrict RPP
    , double **const restrict ROWS_END
    , const int               DIAGONAL_INDEX
    , const int               COL)
{
    double **const START = RPP + 1;

    double *const p_vector = (*RPP) + DIAGONAL_INDEX;

    double const COEF_DENOMINATOR = *(p_vector - 1);

    const int CHUNK_LEN = COL - DIAGONAL_INDEX;

    double **dpp;
    #pragma omp parallel for default(none) private(dpp)
    for (dpp = START; dpp < ROWS_END; ++dpp) {

        double *d_vector = (*dpp) + DIAGONAL_INDEX;

        const double COEF = *(d_vector - 1) / COEF_DENOMINATOR;

        _echelon_annihilate_next_row(

              d_vector

            , p_vector

            , COEF

            , CHUNK_LEN);
    }

}


static void *
_echelon_get_next_pivot(double **const rpp, const int STRT, const int END)
// find the next pivot, return its value
{
    double shared_highest = 0.0;
    int shared_index = STRT;
    #pragma omp parallel default(none) shared(shared_highest, shared_index)
    {
        double highest = 0.0;

        int index = 0;

        int i;

        // each thread finds own highest index
        #pragma omp for
        for (i = STRT; i < END; ++i)

            if (__builtin_fabs(rpp[i][STRT]) > highest)

                highest = __builtin_fabs(rpp[i][STRT]),

                index = i;

        // critical section getting highest value among threads
        #pragma omp flush(shared_highest, shared_index)
        if (highest > shared_highest)

            #pragma omp critical
            if (highest > shared_highest)

                shared_highest = highest,

                shared_index = index;
    }

    double *tmp = rpp[STRT];

    rpp[STRT] = rpp[shared_index];

    rpp[shared_index] = tmp;

    return rpp;
}

/** Private function which reduces the matrix into reduced echelon form by
 *  calling __echelon_annihilation for the number of prime reductions to...
 *  take place.
 */
extern void print_matrix_debug(
        const char *, double **, double ** const, const int);
void
echelon_reduce(
      double **const row_vector
    , const int ROW
    , const int COL)
{
#ifndef NDEBUG
    double time = omp_get_wtime();
#endif

    /* pointer to pointer to the first index of the last row */
    double **const ROWS_LAST = row_vector + ROW;

    /* "row pointer pointer" */
    double **rpp = row_vector;

    /* number of prime/delta eliminations to do */
    const int NUM_DELTA = ROW - 1;

    /* perform elimiation */
    int i;
    // for (i = 0; i < NUM_DELTA; ++i)
    for (i = 0; i < NUM_DELTA; ++i) {
        _echelon_annihilation(
            _echelon_get_next_pivot(row_vector, i, ROW)

            , ROWS_LAST

            , i + 1

            , COL);

            print_matrix_debug("", row_vector, ROWS_LAST, ROW);

            rpp++;

    }

    #ifndef NDEBUG
    printf("\n\t\treduce time: %lf\n", omp_get_wtime() - time);
    #endif
}
