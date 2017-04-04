#include <stdio.h>
#include <stdlib.h>

#define SPEEDUP_PATH "speedup.png"
#define EFFICIENCY_PATH "efficiency.png"

#define BUF_LEN 16

#define WIDTH 1000.0
#define HEIGHT 1000.0
#define OFFSET 100.0
#define STEP (HEIGHT - OFFSET) / (RUNS + 1)

#define TITLE 0
#define Y_LABEL 1
#define X_LABEL 2

enum { SPEEDUP , EFFICIENCY };

#include <plot.h>
#include "types.h"

const char *ef_strs[] = {
      "Efficiency vs Num. Processors"
    , "Efficiency"
    , "Number Of Processors"
};

const char *su_strs[] = {
      "SpeedUp vs Num. Processors"
    , "SpeedUp Ratio"
    , "Number Of Processors"
};

static void draw_x_axis(plPlotter *plotter, int num_threads[RUNS]);
static void _draw_label(plPlotter *, double, double, double, const char *);
static void * plotter_init(int type);

static int draw_effeciency_graph(struct _Data *data);
static int draw_speedup_graph(struct _Data *data);

static void draw_curve(plPlotter *plotter, double speedup[RUNS], int type);
static void draw_text(plPlotter *plotter, int type);
static void draw_y_axis(plPlotter *plotter);

int
generate_plot_png(struct _Data *rundata)
#define NUM_GRAPHS 2
{
    int err;

    err = draw_speedup_graph(rundata);

    err = draw_effeciency_graph(rundata);

    return err;
}

static int
draw_effeciency_graph(struct _Data *data)
{
    plPlotter *plotter = plotter_init(EFFICIENCY);

    draw_curve(plotter, data->efficiency, EFFICIENCY);

    draw_text(plotter, EFFICIENCY);

    draw_x_axis(plotter, data->num_threads);

    draw_y_axis(plotter);

    pl_endpath_r(plotter);

    pl_closepl_r(plotter);

    return pl_deletepl_r(plotter);
}

static int
draw_speedup_graph(struct _Data *data)
{
    plPlotter *plotter = plotter_init(SPEEDUP);

    draw_curve(plotter, data->speedup, SPEEDUP);

    draw_text(plotter, SPEEDUP);

    draw_x_axis(plotter, data->num_threads);

    draw_y_axis(plotter);

    pl_endpath_r(plotter);

    pl_closepl_r(plotter);

    return pl_deletepl_r(plotter);
}


static void
draw_x_axis(plPlotter *plotter, int num_threads[RUNS])
{
    char numbuf[BUF_LEN];

    const double LOC_OFF = OFFSET * 2;

    /* x line */
    pl_fmove_r(plotter, OFFSET, OFFSET);

    pl_fcont_r(plotter, 950, OFFSET);

    int i;
    for (i = 0; i < RUNS; ++i) {
    /* x ticks */

        const double pos = STEP * i + LOC_OFF;

        pl_fmove_r(plotter, pos, OFFSET + 10);

        pl_fcont_r(plotter, pos, OFFSET - 10);

        pl_endsubpath_r(plotter);
    }

    for (i = 0; i < RUNS; ++i) {
    /* x labels */

        const double pos = STEP * i + LOC_OFF;

        snprintf(numbuf, 10, "%i", num_threads[i]);

        _draw_label(plotter,  pos, OFFSET - 30.0,  30.0, numbuf);
    }
}

static void
draw_y_axis(plPlotter *plotter)
#define Y_AXIS_MAX 4
#define OFFSET2 OFFSET * 2
#define HSTEP (HEIGHT - OFFSET) / (Y_AXIS_MAX)
{
    /* y line */
    pl_fmove_r(plotter, OFFSET, OFFSET);

    pl_fcont_r(plotter, OFFSET, HEIGHT * 0.8);

    char numbuf[BUF_LEN];

    int i;
    for (i = 1; i < Y_AXIS_MAX; ++i) {
    /* y ticks*/

        const double pos = HSTEP * i + OFFSET;

        pl_fmove_r(plotter, OFFSET + 10, pos);

        pl_fcont_r(plotter, OFFSET - 10, pos);

        pl_endsubpath_r(plotter);
    }

    for (i = 0; i < Y_AXIS_MAX; ++i) {
    /* y labels */

        const double pos = HSTEP * i + OFFSET;

        snprintf(numbuf, 10, "%.1lf", (double)i);

        _draw_label(plotter, OFFSET - 40, pos, 30.0, numbuf);
    }
}

static void
draw_text(plPlotter *plotter, int type)
#define LABEL_X_COORD(X) ((X) - (X) / 2 + 10)
#define L_SZ 30.0
{
    if (type == EFFICIENCY)

        /* x label */
        _draw_label(plotter, WIDTH / 2, 20, L_SZ, ef_strs[X_LABEL]),

        _draw_label(

              plotter

            , LABEL_X_COORD(pl_flabelwidth_r(plotter, ef_strs[Y_LABEL]))

            , HEIGHT - HEIGHT / 6

            , L_SZ

            , ef_strs[Y_LABEL]),

        /* title */
        _draw_label(

              plotter

            , WIDTH / 2

            , HEIGHT - HEIGHT / 10

            , 50.0

            , ef_strs[TITLE]);

    else /* speedup graph */

        /* x label */
        _draw_label(plotter, WIDTH / 2, 20, L_SZ, su_strs[X_LABEL]),

        /* y label */
        _draw_label(
              plotter

            , LABEL_X_COORD(pl_flabelwidth_r(plotter, su_strs[Y_LABEL]))

            , HEIGHT - HEIGHT / 6

            , L_SZ

            , su_strs[Y_LABEL]),

        /* title */
        _draw_label(
              plotter
            , WIDTH / 2
            , HEIGHT - HEIGHT / 10
            , 50.0
            , su_strs[TITLE]);
}

static void
_draw_label(plPlotter *plotter, double x, double y, double size, const char *s)
{
    pl_ffontsize_r(plotter, size);

    pl_fmove_r(plotter, x, y);

    pl_alabel_r(plotter, 'c', 'c', s);
}


static void
draw_curve(plPlotter *plotter, double speedup[RUNS], int type)
#define TO_X_COORD(INTEGRAL) ((INTEGRAL) * STEP + OFFSET2)
#define TO_Y_COORD(DPF) ((DPF) * HSTEP + OFFSET)
{
    pl_ffontsize_r(plotter, 30.0);
    int i;

    if (type == EFFICIENCY)

        pl_fmove_r(plotter, OFFSET2, HSTEP + OFFSET);

    else

        pl_fmove_r(plotter, OFFSET2, TO_Y_COORD(speedup[0]));

    for (i = 0; i < RUNS; ++i)

        pl_fcont_r(
              plotter
            , TO_X_COORD(i)
            , TO_Y_COORD(speedup[i]) // - 10
            );

    char numbuf[BUF_LEN];

    pl_fmove_r(plotter, OFFSET2, speedup[0] * OFFSET);

    for (i = 1; i < RUNS; ++i)

        snprintf(numbuf, BUF_LEN, "%.3lf", speedup[i]),

        pl_fmove_r(plotter
            , TO_X_COORD(i)
            , TO_Y_COORD(speedup[i]) + 30
            ),

        pl_alabel_r(plotter, 'c', 'c', numbuf);
}

static void *
plotter_init(int type)
{
    plPlotterParams *plotter_params = pl_newplparams();

    pl_setplparam(plotter_params, "BITMAPSIZE", NULL);

    plPlotter *plotter;

    FILE *fp;

    if (type == SPEEDUP)

        fp = fopen(SPEEDUP_PATH, "w");

    else

        fp = fopen(EFFICIENCY_PATH, "w");

    if (!fp)

        return NULL;

    plotter = pl_newpl_r("png", stdin, fp, stderr, plotter_params);

    pl_openpl_r(plotter);

    /* coordinate system */
    pl_fspace_r(plotter, 0.0, 0.0, WIDTH, HEIGHT);

    pl_flinewidth_r(plotter, 2.0);

    pl_pencolorname_r(plotter, "black");

    pl_fillcolorname_r(plotter, "light blue");

    pl_erase_r(plotter);

    pl_orientation_r(plotter, 1);

    return plotter;
}

