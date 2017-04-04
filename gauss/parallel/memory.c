/** @file dmalloc.c */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <unistd.h>
#include "gauss_types.h"

#define D_BYTES(L) ((L) * sizeof(double) + sizeof(double))
#define DP_BYTES(L) ((L) * sizeof(double *) + sizeof(double *))

static void *__attribute__ ((malloc))
_double_t_memalloc(const size_t LEN, const size_t CLS)
{
   double *ret = NULL;

   posix_memalign((void **)&ret, CLS, D_BYTES(LEN));

   return ret;
}

static void *__attribute__ ((malloc))
_double_t_ptralloc(const size_t LEN, const size_t CLS)
{
   double **ret = NULL;

   posix_memalign((void *)&ret, CLS, DP_BYTES(LEN));

   return ret;
}

static size_t
_vectors_memalloc(struct _ArrayData *ret, const size_t N, const size_t CLS)
{
    if (ret->echelon || ret->variables || ret->original || ret->rows)

        return 0;

    const size_t LEN = (N+1) * N;

    ret->echelon = _double_t_memalloc(LEN, CLS);

    ret->ECHELON_LAST = &ret->echelon[LEN - 1];

    ret->rows = _double_t_ptralloc(N, CLS);

    ret->ROWS_LAST = &ret->rows[N];

    ret->variables = _double_t_memalloc(N, CLS);

    ret->VARIABLES_LAST = &ret->variables[N - 1];

    ret->original = _double_t_memalloc(LEN, CLS);

    return LEN;
}

static void
_set_rows(struct _ArrayData *ret, const size_t N)
{
    double *ep = ret->echelon;

    const size_t COL = N + 1;

    size_t i;
    for (i = 0; i < N; ++i) {

        ret->rows[i] = ep;

        ep += COL;

    }
}

int
vectors_init(struct _ArrayData *ret, const size_t N, const size_t CLS)
{
    size_t len = _vectors_memalloc(ret, N, CLS);

    if (!len)

        return 0;

    srand48(getpid());

    int i = (int)len;

    while (--i)

        ret->echelon[i] = drand48() * 2.0 - 1.0;

    ret->echelon[i] = drand48() * 2.0 - 1.0;

    __builtin_memcpy(ret->original, ret->echelon, sizeof(double) * len);

    _set_rows(ret, N);

    return 1;
}

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
