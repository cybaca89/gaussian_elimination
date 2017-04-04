extern void gaussian_finalize(void);
extern void gaussian_init(int);
extern void gaussian_execute(void);

#include <stdlib.h>

#define N 5

#include <sys/time.h>       /* idk */
#include <sys/resource.h>   /* getrusage() */

void print_rusage(const struct rusage *);

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

void
print_rusage(const struct rusage *stats)
{
    __builtin_printf(
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
