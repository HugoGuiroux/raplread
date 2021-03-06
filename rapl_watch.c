#include <stdio.h>
#include <time.h>

#include "rapl_read.h"

#define MSEC_IN_SEC 1000
#define NSEC_IN_MSEC 1000
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s interval_in_ms\n", argv[0]);
        return 0;
    }

    uint32_t t = atol(argv[1]);
    struct timespec s = { .tv_sec = t / MSEC_IN_SEC,
                          .tv_nsec = (t % MSEC_IN_SEC) * NSEC_IN_MSEC };
    RR_INIT_ALL();

    while (1) {
        RR_START_UNPROTECTED_ALL();
        if (nanosleep(&s, NULL) == -1) {
            perror("Nanosleep returned -1");
            return -1;
        }

        RR_STOP_UNPROTECTED_ALL();
	rapl_stats_t s;
	RR_STATS(&s);
	double total = s.energy_package[NUMBER_OF_SOCKETS] + s.energy_dram[NUMBER_OF_SOCKETS];
	if (total <= 0)
 	    total = -1;
        fprintf(stderr, "%f\n", total);

    }

    return 0;
}
