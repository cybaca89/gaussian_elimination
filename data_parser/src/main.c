#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define DEFAULT_PATH "out.txt"
#define BUF_LEN 64
/*
#define RUNS 7
#define SET_LEN 3
#define BUF_LEN 64

struct _Data {

    struct {

        double times[SET_LEN];

        double l2norms[SET_LEN];

    } sets[RUNS];

    double times[RUNS];

    double speedup[RUNS];

    double efficiency[RUNS];

    int num_threads[RUNS];

};
*/

int get_data_from_file(const char *, struct _Data *);
void calculate_fastest_times(struct _Data *rundata);
void calculate_speedups(struct _Data *rundata);
void calculate_efficiency(struct _Data *rundata);
void print_inputs(struct _Data *rundata);
void print_table(struct _Data *rundata);
extern int generate_plot_png(struct _Data *data);

int
main(int argc, char **argv)
{
    char filepath[64];

    struct _Data rundata;

    if (argc < 2)

        strcpy(filepath, DEFAULT_PATH);

    else

        strcpy(filepath, argv[1]);

    if (!get_data_from_file(filepath, &rundata))

        fprintf(stderr, "Unable to locate file \"%s\"\n", filepath),

        exit (EXIT_FAILURE);

    calculate_fastest_times(&rundata);

    calculate_speedups(&rundata);

    calculate_efficiency(&rundata);

    print_inputs(&rundata);

    print_table(&rundata);

    generate_plot_png(&rundata);
}

int
get_data_from_file(const char *filepath, struct _Data *rundata)
{

    FILE *fp = fopen(filepath, "r");

    if (!fp)

        return 0;

    char line[BUF_LEN];

    int runs;
    for (runs = 0; runs < RUNS; ++runs) {

        fgets(line, BUF_LEN, fp);

        sscanf(line, "threads: %i", &rundata->num_threads[runs]);

        int i;
        for (i = 0; i < SET_LEN; ++i)

            fgets(line, BUF_LEN, fp),

            sscanf(line, "TIME: %lf", &rundata->sets[runs].times[i]);

        for (i = 0; i < SET_LEN; ++i)

            fgets(line, BUF_LEN, fp),

            sscanf(line, "L2 norm: %le", &rundata->sets[runs].l2norms[i]);
    }

    fclose(fp);

    return 1;
}

void
calculate_fastest_times(struct _Data *rundata)
{
    int run;
    for (run = 0; run < RUNS; ++run) {

        double fastest = rundata->sets[run].times[0];

        int set;
        for (set = 1; set < SET_LEN; ++set)

            if (rundata->sets[run].times[set] < fastest)

                fastest = rundata->sets[run].times[set];

        rundata->times[run] = fastest;
    }
}

void
calculate_speedups(struct _Data *rundata)
{
    int run;
    for (run = 0; run < RUNS; ++run)

        rundata->speedup[run]

        = rundata->times[0]

        / rundata->times[run];
}

void
calculate_efficiency(struct _Data *rundata)
{
    int run;
    for (run = 0; run < RUNS; ++run)

        rundata->efficiency[run]

        = rundata->speedup[run]

        / rundata->num_threads[run];
}

void
print_inputs(struct _Data *rundata)
{
    int runs;
    for (runs = 0; runs < RUNS; ++runs) {

        printf("threads: %i\n", rundata->num_threads[runs]);

        printf("times:   ");

        int i;
        for (i = 0; i < SET_LEN; ++i)

            printf("%lf        ", rundata->sets[runs].times[i]);

        printf("\nl2norms: ");

        for (i = 0; i < SET_LEN; ++i)

            printf("%.10le  ", rundata->sets[runs].l2norms[i]);

        printf("\n\n");
    }
}

void
print_table(struct _Data *rundata)
{
    /*
    printf(

    "threads: %i\n"

    "minumum time: %lf\n\n"

    , rundata->num_threads[0]

    , rundata->times[0]

    );
    */

    int runs;
    for (runs = 0; runs < RUNS; ++runs) {

        printf(

        "threads: %i\n"

        "minumum time: %lf\n"

        "speedup:      %lf\n"

        "efficiency:   %lf\n\n"

        , rundata->num_threads[runs]

        , rundata->times[runs]

        , rundata->speedup[runs]

        , rundata->efficiency[runs]

        );
    }
}
