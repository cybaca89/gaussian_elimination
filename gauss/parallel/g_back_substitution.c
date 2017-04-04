/** linear solver using Gaussian elimination with partial pivoting
 *  run with 1, 2, 5, 10, 20, 30, and 40 cores
 */
#include <stdio.h>
#include <omp.h>

void
back_substitution(
      double **const restrict rows
    , double **const restrict ROWS_LAST
    , double *const VARIABLES_LAST
    , const int ROW
    )
{
#ifndef NDEBUG
double time = omp_get_wtime();
#endif

    double **const START = ROWS_LAST - 2;

    double **const END = rows - 1;

    const int A_END = ROW - 1;

    VARIABLES_LAST[0] = ROWS_LAST[-1][ROW] / ROWS_LAST[-1][A_END];

    int i = -1;

    double **itr;
    for (itr = START; itr > END; --itr) {

        double *const e_ptr = &(*itr)[A_END];

        double sub = e_ptr[1];

        int j;
        #pragma omp parallel for lastprivate(j) reduction(-:sub)
        for (j = 0; j > i; --j)

            sub -= VARIABLES_LAST[j] * e_ptr[j];

        VARIABLES_LAST[i--] = sub / e_ptr[j];
    }

    #ifndef NDEBUG
    printf("\n\t\tback_sub time: %lf\n", omp_get_wtime() - time);
    #endif
}
