#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <getopt.h>

#include "rapl_read.h"

int main(int argc, char *argv[]) {
    char *output = NULL;
    int option = 0;
    while ((option = getopt(argc, argv, "o:")) != -1) {
        switch (option) {
        case 'o':
            output = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o output] -- command to execute\n",
                    argv[0]);
            return 0;
        }
    }

    if (optind == argc) {
        fprintf(stderr, "Usage: %s command to execute\n", argv[0]);
        return 0;
    }

    FILE *fd = stdout;
    if (output != NULL) {
        fd = fopen(output, "w");
        if (fd == NULL) {
            perror("output");
            return -1;
        }
    }

    RR_INIT_ALL();
    RR_START_UNPROTECTED_ALL();

    pid_t c = fork();
    if (c < 0) {
        perror("fork");
        return -1;
    } else if (c == 0) {
        if (execvp(argv[optind], &argv[optind]) < 0) {
            perror("execvp");
            return -1;
        }
    }

    if (wait(NULL) < 0) {
        perror("wait");
        return -1;
    }

    RR_STOP_UNPROTECTED_ALL();
    rapl_stats_t s;
    RR_STATS(&s);
    double total =
        s.energy_package[NUMBER_OF_SOCKETS] + s.energy_dram[NUMBER_OF_SOCKETS];
    if (total <= 0)
        total = -1;
    fprintf(fd, "%f\n", total);
    RR_PRINT_UNPROTECTED(RAPL_PRINT_ENE);

    fclose(fd);
    return 0;
}
