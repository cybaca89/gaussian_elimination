/** @file public_gauss.h
 *  local/private gaussian elimination header file
 */

#include "gauss_types.h"

/** macro defininations */

/* maximum matrix height allowed for printing matrix values afte
 * elimination. For debugging, I want to be able to see how different sizes.....
 * and whether odd or event heights affect the output/time/accuracy
 */
#ifdef NDEBUG // from assert.h
#define EPSILON 5
#else
#define EPSILON 10
#endif



/** private variables for runtime */

static struct _ArrayData _vectors = {
      NULL, NULL
    , NULL, NULL
    , NULL, NULL
    , NULL       };

static struct _Stats _stats = { 0.0, 0.0, 0, 0 };

// set to 1 if initialized correctly
static int _ready = 0;

static /* const */ int _COL = 0, _ROW = 0;

// cache line size for heap alignment
static /* const */ size_t _CLS = 0;





/** private functions */
/** @file echelon.c
 *  Reduce the matrix into upper right/echelon form.
 *  Loop of length <height of matrix> or _ROW - 1 which calls
 *  __echelon form prime values one for each prime reduction on the given
 *  matrix.
 *  Keeps track of the location of the next pivot, using the return value
 *  from __echelon_form_prime_values()
 *
 *  To impliment partial pivoting, I will need to implement a data structure
 *  or procedure to keep track of the order of iteration of the matrix without
 *  incurring too much concurrency/cache data overhead
 *
 *  @param echelon pointer to start of array which holds data for the echelon
 *         matrix
 *  @param ROW height of the matrix
 */
extern void
echelon_reduce(
      double **     // rows
    , const int     // ROW
    , const int     // COL
);

/** @file back_substitution.c
 *  Backward substitution on upper matrix to find x values
 *  Iterates backwards from the end of _vectors.echelon to find
 *  the x values for the echelon matrix, and stores them in _vectors.variables
 *  backwards so that _vectors.variables[0] will correspond to x0,
 *  _vectors.variables [1] will correspond to x1, and so on.
 *  Meanwhile, it uses previously found values of _vectors.variables to find
 *  the next values.
 */
extern void
back_substitution(
      double **const restrict // rows
    , double **const restrict // rows last
    , double *const // variables
    , const int     // COL
    );

/** @file output.c
 *  prints original matrix and calculated x values.
 */
extern void
print_values(
      double *const /* original */
#   ifdef NDEBUG
    , double *const /* variables */
#   endif
    , const int /* ROW */
    , const int /* COL */);

/** @file output.c
 *  prints runtime info
 */
extern void
print_stats(struct _Stats *);

/** @file validity_check.c
 *  ensures calculations of x values are accurate
 */
extern double
l2_norm_validity_check(
      double *const /* original */
    , double *const /* variables */
    , const int     /* ROW */
    , const int     /* COL */);

/** @file memory.c
 */
extern void
vectors_free(struct _ArrayData *);

extern int
vectors_init(struct _ArrayData *, const size_t, const size_t);
