/* linear solver using Gaussian elimination with partial pivoting */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for getpid(), sysconf() */
#include <assert.h>
#include <math.h>
#include <sys/time.h>       /* idk */
#include <sys/resource.h>   /* getrusage() */
#define N 5

void print_rusage(const struct rusage *);
struct {

    double *eche;
    double **rows;
    double *vars;
    double *orig;

} _g;

int _col, _row;
int _READY = 0;

double
_pivot(const int beg, const int end)
{
    double highest = 0.0;
    int index = 0;
    int i;
    for (i = beg; i < end; ++i)

        if( fabs(_g.rows[i][beg]) > highest)

            highest = fabs(_g.rows[i][beg]),

            index = i;

    double *tmp = _g.rows[beg];

    _g.rows[beg] = _g.rows[index];

    _g.rows[index] = tmp;

    return _g.rows[beg][beg];
}


void
_print_matrix(const char *name, double *matrix)
{
    int i, j;

    printf("\t\n\n%s\n", name);

    for (i = 0; i < _row; ++i) {

        printf("\t");

        char var = 97;

        for (j = 0; j < _col - 1; ++j, ++var)

            printf("%+-.2lf%c ", matrix[j], var);

        printf(" = %.2lf\n", matrix[j]);

        matrix += _col;

    }
}

void
_generate_echelon(void)
{
    const int MAXP = _row - 1;

    double **const low = _g.rows + 1;

    double **const hig = _g.rows;

    int i, j, k;

    for (i = 0; i < MAXP; ++i) {

        double bottom = _pivot(i, MAXP);

        for (j = i; j < MAXP; ++j) {

            const double COEF = low[j][i] / bottom;

            for (k = i + 1; k < _col; ++k)

                low[j][k] -= COEF * hig[i][k];

        }
    }
}

void
back_substitution(void)
{
    double **const START = _g.rows + _row - 2;

    double **const END = _g.rows - 1;

    const int A_END = _row - 1;

    double *const VARS_LAST = _g.vars + A_END;

    VARS_LAST[0] = _g.rows[A_END][_row] / _g.rows[A_END][A_END];

    int i = -1;

    double **itr;

    for (itr = START; itr > END; --itr) {
        double *const e_ptr = &(*itr)[A_END];

        double sub = e_ptr[1];

        int j;
        for (j = 0; j > i; --j)

            sub -= VARS_LAST[j] * e_ptr[j];

        VARS_LAST[i--] = sub / e_ptr[j];
    }
}

void
_l2_norm_validity_check(void)
{
    double l2_norm = 0.0;

    double *err_arr = malloc(sizeof(double) * (size_t)_row);

    double *restrict orig_ptr = _g.orig;

    double *restrict vars_ptr = _g.vars;

    int i, j;

    for (i = 0; i < _row; ++i) {

        double b = 0.0;

        for (j = 0; j < _row; ++j)

            b += orig_ptr[j] * _g.vars[j];

        err_arr[i] = fabs(b - orig_ptr[j]);

        l2_norm += err_arr[i] * err_arr[i];

        orig_ptr += _col;
    }

    for (i = 0; i < _row; ++i)

        if (err_arr[i] > 0.0001)

            fprintf(stderr, "COMPUTATION ERROR: diff is %lf\n", err_arr[i]);

    printf("residual_vector: %.10le\n", sqrt(l2_norm));

    free(err_arr);
}

void
gaussian_init(int n)
{
    _g.eche = NULL;

    _g.vars = NULL;

    _g.orig = NULL;

    _g.rows = NULL;

    size_t cols = (size_t)(n + 1);

    size_t rows = (size_t)n;

    _col = (int)cols;

    _row = (int)rows;

    int len = _col * _row;

    size_t cls = (unsigned)sysconf(_SC_LEVEL1_DCACHE_LINESIZE);

    // allocate heap memory
    posix_memalign((void **)&_g.eche, cls, sizeof(double) * rows * cols);

    posix_memalign((void *)&_g.rows, cls, sizeof(double) * rows);

    posix_memalign((void **)&_g.vars, cls, sizeof(double) * rows);

    posix_memalign((void **)&_g.orig, cls, sizeof(double) * rows  * cols);

    // generate random matrix for elimination
    #define PF_READ 0
    #define PF_WRITE 1
    #define PF_NON_TEMPORAL 0
    #define PF_LOW_TEMPORAL 1
    #define PF_MID_TEMPORAL 2
    #define PF_HI_TEMPORAL 3

    int i;

    int j;

    srand48(getpid());

    int cds = (int)(cls / sizeof(double));

    if (len % cds)

        cds = 1;

    for (j = 0; j < len; j += cds) {

        __builtin_prefetch(&_g.orig[j], PF_WRITE, PF_NON_TEMPORAL);

        for (i = j; i < j + cds; ++i)

            _g.orig[i] = drand48() * 2.0 - 1.0;

    }

    // copy orig matrix to echelon matrix
    __builtin_memcpy(_g.eche, _g.orig, sizeof(double) * rows * cols);

    double *ep = _g.eche;

    for (i = 0; i < n; ++i)

        _g.rows[i] = ep, ep += _col;

    _READY = 1;

}

void
gaussian_execute()
{
    if (!_READY)

        return;

    if (_row < 10)

        _print_matrix("orig:", _g.orig);

    _generate_echelon();

    back_substitution();

    _l2_norm_validity_check();
}

void
gaussian_finalize(void)
{
    if (_g.rows)

        free(_g.rows);

    if (_g.eche)

        free(_g.eche);

    if (_g.vars)

        free(_g.vars);

    if (_g.orig)

        free(_g.orig);
}

void
print_rusage(const struct rusage *stats)
{
    printf(

    "user CPU time         : %lf\n"

    "system CPU time       : %lf\n"

    "max resdident set size: %ld\n"

    "minor page faults     : %ld\n"

    "major page faults     : %ld\n\n\n"

    , (double)stats->ru_utime.tv_sec

    + (double)stats->ru_utime.tv_usec / 1000000.0

    , (double)stats->ru_stime.tv_sec

    + (double)stats->ru_stime.tv_usec / 1000000.0

    , stats->ru_maxrss

    , stats->ru_minflt

    , stats->ru_majflt

    );
}

int
main(int argc, char **argv)
/** PROGRAM START **/
{
    int n = N;

    if (argc > 1)

        n = atoi(argv[1]);

    gaussian_init(n);

    gaussian_execute();

    gaussian_finalize();

    struct rusage stats;

    getrusage(RUSAGE_SELF, &stats);

    print_rusage(&stats);

    exit(EXIT_SUCCESS);
}
