/** linear solver using Gaussian elimination with partial pivoting
 *  run with 1, 2, 5, 10, 20, 30, and 40 cores
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
// #include <assert.h>
#include <stdio.h>  /* printf */
#include <stdlib.h> /* malloc, free */
#include <math.h>   /* sqrt */
#include <omp.h>

double
l2_norm_validity_check(
      double *const original
    , double *const variables
    , const int ROW
    , const int COL)
{
#   ifndef NDEBUG
    double time = omp_get_wtime();
#   endif

    double *orig_ptr = original;
    double *vars_ptr = variables;

    double *err_arr = malloc(sizeof(double) * (size_t)ROW);

    double l2_norm = 0.0;

    int i;

    for (i = 0; i < ROW; ++i) {

        double b = 0.0;

        int j;
        // #pragma omp parallel for lastprivate(j) reduction(+:b)
        for (j = 0; j < ROW; ++j)

            b += orig_ptr[j] * vars_ptr[j];

        err_arr[i] = fabs(b - orig_ptr[j])

        , l2_norm += err_arr[i] * err_arr[i]

        , orig_ptr += COL;
    }

    for (i = 0; i < ROW; ++i)

        if (err_arr[i] > 0.01) {

            fprintf(stderr, "COMPUTATION ERROR: diff is %lf\n", err_arr[i]);

            break;

        }


#   ifndef NDEBUG
    printf("\n\t\tl2_check time: %lf\n", omp_get_wtime() - time);
#   endif

    free(err_arr);

    return sqrt(l2_norm);
}
