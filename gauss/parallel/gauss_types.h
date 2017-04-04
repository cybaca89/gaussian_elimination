/** @file gauss_types.h
 *  implimentation structure definitions to be used in gaussian elimination
 */

#ifndef _GAUSS_TYPES_H_
#define _GAUSS_TYPES_H_
struct _Stats {

    double time;
    double l2_norm;
    int cores;
    int threads;

};

/** all pointers should be treated as *const */
struct _ArrayData {

    double *echelon, *ECHELON_LAST;
    double **rows, **ROWS_LAST;
    double *variables, *VARIABLES_LAST;
    double *original;

};
#endif
