/** @file guassian_elimination.c
 *  linear solver using Gaussian elimination with partial pivoting
 *  run with 1, 2, 5, 10, 20, 30, and 40 cores
 */

#define _GNU_SOURCE /* posix_align, drand48 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>   /* sqrt */
#include <unistd.h> /* getpid */
#include <omp.h>

/** macro defininations */
#define DEFAULT_N 4
#define EPSILON 5

/** global variables */
struct _Stats {
    double time;
    double l2_norm;
    int cores;
    int threads;
}
_stats = { 0.0, 0.0, 0, 0 };

struct _ArrayData {

    double *echelon;
    double *ECHELON_LAST;

    double **rows;
    double **ROWS_LAST;

    double *variables;
    double *VARIABLES_LAST;

    double *original;
}
_vectors = { NULL, NULL , NULL, NULL , NULL, NULL , NULL };

/** Annihilation of variables to get delta/prime values for rows for use in
 *  _echelon_reduce().
 *
 *  @param COEF_DENOMINATOR value at the first index of the pivot row
 *  @param RPP row pointer, pointer to location of the pivot row
 *  @param ROW_END start location of annihilation from RP
 *  @param DIAGONAL_INDEX the next column on which to start annihilation
 *  @param COL_LEN witdth of the matrix
 *  @return pointer to the next row
 */
void *
_echelon_annihilate(
      const double            COEF_DENOMINATOR
    , double **const restrict RPP
    , double **const restrict ROWS_END
    , const int               DIAGONAL_INDEX
    , const int               COL_LEN)
{
    double **const START = RPP + 1;

    double *const pivot_row = (*RPP) + DIAGONAL_INDEX;

    double **cur_row;
    #pragma omp parallel for default(none) private(cur_row)
    for (cur_row = START; cur_row < ROWS_END; ++cur_row) {

        double *const local_row = (*cur_row) + DIAGONAL_INDEX;

        const double COEF = cur_row[0][DIAGONAL_INDEX - 1] / COEF_DENOMINATOR;

        const int CHUNK_LEN = COL_LEN - DIAGONAL_INDEX;

        int i;
        for (i = 0; i < CHUNK_LEN; ++i)

            local_row[i] -= COEF * pivot_row[i];
    }

    return RPP + 1;
}

/** Peform Partial Pivoting on the next column, reorder the array of pointers
 *  and return the value at the pivot's first index while we're at it.
 *  @param rpp Pointer to array of pointers with the locations of the
 *         beginning of each row.
 *  @param STRT, index of the next column to search through for the highest
 *         pivot value.
 *  @param END, base case for the search loop
 */
double
_echelon_pivot(double **const rpp, const int STRT, const int END)
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

        #pragma omp critical
        if (highest > shared_highest)

            shared_highest = highest,

            shared_index = index;

    }

    double *tmp = rpp[STRT];

    rpp[STRT] = rpp[shared_index];

    rpp[shared_index] = tmp;

    return rpp[STRT][STRT];
}

/** Reduce the matrix into upper right/echelon form.
 *  Calls on helper functions to do partial pivoting and annihilate rows
 *  in a loop.
 *  @param row_vector,array of pointers the rows of the echelon matrixc
 *  @param ROW, height of the matrix.
 *  @param COL, width of the matrix.
 */
void
echelon_reduce(double **const row_vector, const int ROW, const int COL)
{

    /* pointer to pointer to the first index of the last row */
    double **const ROWS_LAST = row_vector + ROW;

    /* "row pointer pointer" */
    double **rpp = row_vector;

    /* number of pivots */
    const int NUM_DIAG = ROW - 1;

    /* perform elimiation */
    int i;
    for (i = 0; i < NUM_DIAG; ++i)

        rpp = _echelon_annihilate(
              _echelon_pivot(row_vector, i, ROW), rpp, ROWS_LAST, i+1, COL);

}

/** Backward substitution on upper matrix to find x values
 *  @param rows, array of pointers holding the row locations for the echelon
 *         matrix
 *  @param ROWS_LAST, pointer to pointer to the the last row of the echelon
 *         matrix.
 *  @param VARIABLES_LAST, pointer to last index of the variables array
 *  @param ROW, height of the matrix.
 */
void
back_substitution(
      double **const restrict rows
    , double **const restrict ROWS_LAST
    , double *const VARIABLES_LAST
    , const int ROW)
{
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
        #pragma omp parallel for schedule(dynamic, 8) \
        lastprivate(j) reduction(-:sub)
        for (j = 0; j > i; --j)

            sub -= VARIABLES_LAST[j] * e_ptr[j];

        VARIABLES_LAST[i--] = sub / e_ptr[j];
    }

}

/** ensures calculations of x values are accurate */
double
l2_norm_validity_check(
      double *const original
    , double *const variables
    , const int ROW
    , const int COL)
{
    double *orig_ptr = original;

    double *err_arr = malloc(sizeof(double) * (size_t)ROW);

    double l2_norm = 0.0;

    int i;

    for (i = 0; i < ROW; ++i) {

        double b = 0.0;

        int j;
        for (j = 0; j < ROW; ++j)

            b += orig_ptr[j] * variables[j];

        err_arr[i] = fabs(b - orig_ptr[j]);

        l2_norm += err_arr[i] * err_arr[i];

        orig_ptr += COL;
    }

    free(err_arr);

    return sqrt(l2_norm);
}

/**  Allocator for am alligned block of memory the size of
 *   LEN + 1 doubles, aligned to CLS bytes.
 *
 */ 
void *__attribute__ ((malloc))
_vectors_memalloc(const size_t LEN, const size_t CLS)
#define D_BYTES(L) ((L) * sizeof(double) + sizeof(double))
{
   double *ret = NULL;

   posix_memalign((void **)&ret, CLS, D_BYTES(LEN));

   return ret;
}

/** Allocator for alligned block of memory the size of LEN + 1 doubles.
 *
 */
void *__attribute__ ((malloc))
_vectors_ptralloc(const size_t LEN, const size_t CLS)
#define DP_BYTES(L) ((L) * sizeof(double *) + sizeof(double *))
{
   double **ret = NULL;

   posix_memalign((void *)&ret, CLS, DP_BYTES(LEN));

   return ret;
}

/** Allocates and memory and sets pointers for _vectors data structure.
 *
 */
size_t
_vectors_alloc(struct _ArrayData *ret, const size_t N)
{
    if (ret->echelon || ret->variables || ret->original || ret->rows)

        return 0;

    size_t CLS = (unsigned)sysconf(_SC_LEVEL1_DCACHE_LINESIZE);

    const size_t LEN = (N+1) * N;

    ret->echelon = _vectors_memalloc(LEN, CLS);

    ret->ECHELON_LAST = &ret->echelon[LEN - 1];

    ret->rows = _vectors_ptralloc(N, CLS);

    ret->ROWS_LAST = &ret->rows[N];

    ret->variables = _vectors_memalloc(N, CLS);

    ret->VARIABLES_LAST = &ret->variables[N - 1];

    ret->original = _vectors_memalloc(LEN, CLS);

    return LEN;
}

/** Intializes the "rows" array to hold addresses to front of each row in the
 *  echelon matrix.
 */
void
_vectors_set_rows(struct _ArrayData *ret, const size_t N)
{
    double *ep = ret->echelon;

    const size_t COL = N + 1;

    size_t i;
    for (i = 0; i < N; ++i) {

        ret->rows[i] = ep;

        ep += COL;

    }
}

/** Initializes the starting matrix to random values between -1,000,000
 *  and +1,000,000 and keeps a seperate copy stored for later use.
 *
 */
int
vectors_init(struct _ArrayData *ret, const size_t N)
{
    size_t len = _vectors_alloc(ret, N);

    if (!len)

        return 0;

    srand48(getpid());

    int i = (int)len;

    while (--i)

        ret->echelon[i] = drand48() * 2.0e6 - 1.0e6;

    ret->echelon[i] = drand48() * 2.0e6 - 1.0e6;

    __builtin_memcpy(ret->original, ret->echelon, sizeof(double) * len);

    _vectors_set_rows(ret, N);

    return 1;
}

/** Frees all dynamic memory that needed to be used before exiting.
 *
 */
void
vectors_free(struct _ArrayData *vecs)
{
    if (vecs->original)

        free(vecs->original);

    if (vecs->variables)

        free(vecs->variables);

    if (vecs->rows)

        free(vecs->rows);

    if (vecs->echelon)

        free(vecs->echelon);
}

/** prints runtime info */
void
print_stats(struct _Stats *stats)
{
    printf("\n"

    "\n\tcores:   %d\n"

    "\n\tthreads: %i\n"

    "\n\tTIME:    %lf seconds\n"

    "\n\tL2 norm: %.10le\n"

    , stats->cores

    , stats->threads

    , stats->time

    , stats->l2_norm);
}

/** prints original matrix and calculated x values.  */
void
print_values(
      double *const original
    , double *const variables
    , const int ROW
    , const int COL)
{
    int i, j = 0, col = 0;

    printf("\noriginal matrix:");

    for (i = 0; i < ROW; ++i) {

        col += COL;

        printf("\n");

        while (j < col - 1)

            printf("%+-.10le ", original[j++]);

        printf(" | %+-.10le ", original[j++]);
    }

    printf("\n\nx values:\n");

    for (i = 0; i < ROW; ++i)

        printf("x%d = %.10le\n", i + 1, variables[i]);
}

/** Calls each function for gaussian elimination in the order that they need to
 *  be called, and collects the runtime timestamps.
 *
 */
void
gaussian_elimination(int ROW, int COL)
{
    /* start time */
    _stats.time = omp_get_wtime();

    /* reduce matrix + partial pivoting */
    echelon_reduce(_vectors.rows, ROW, COL);

    /* backward substitution on upper triangle */
    back_substitution(
          _vectors.rows, _vectors.ROWS_LAST, _vectors.VARIABLES_LAST, ROW);

    /* get time */
    _stats.time = omp_get_wtime() - _stats.time;

    /* get l2_norm */
    _stats.l2_norm = l2_norm_validity_check(
          _vectors.original, _vectors.variables, ROW, COL);

    if (ROW < EPSILON)

        print_values(_vectors.original, _vectors.variables, ROW, COL);

    print_stats(&_stats);

    vectors_free(&_vectors);
}

/** Calls functions needed to instantiate / initialize memory / data that will
 *  be used at runtime.
 *
 */
void
program_init(const int N)
{
    /* allocate vectors on heap and instatiate with random numbers */
    if (!vectors_init(&_vectors, (size_t)N))

        fprintf(stderr, "Failed to initialize vector data\n"),

        exit(EXIT_FAILURE);

    /* omp info for end of program */
    #pragma omp parallel default(none) shared(_stats)

    #pragma omp master

        _stats.threads = omp_get_num_threads(),

        _stats.cores = omp_get_num_procs();

    gaussian_elimination(N, N+1);
}

int
main(int argc, char **argv)
/** PROGRAM START **/
#define NOT_ENOUGH 1
#define TOO_MUCH 9000
{
    int n = DEFAULT_N;

    if (argc > 1)

        n = atoi(argv[1]);

    if (n < NOT_ENOUGH)

        fprintf(stderr, "0 and below is too small\n"),

        exit(EXIT_FAILURE);

    else if (n > TOO_MUCH)

        fprintf(stderr, "\n\nITS OVER 9000!!!!\n"),

        exit(EXIT_FAILURE);

    // gaussian elimination
    program_init(n);

    exit(EXIT_SUCCESS);
}
