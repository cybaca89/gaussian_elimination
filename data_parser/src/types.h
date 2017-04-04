#define RUNS 7
#define SET_LEN 3

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
