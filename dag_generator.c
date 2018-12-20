#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>

#define MIN_PER_RANK 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 30
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 60
#define PERCENT 70     /* Chance of having an Edge.  */

bool visited[100000];


int main(void) {
    int i, j, k, nodes = 0;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

    srand(time_in_micros);

    int ranks = MIN_RANKS
                + (rand() % (MAX_RANKS - MIN_RANKS + 1));

    printf("digraph {\n");
    visited[0] = true;

    for (i = 0; i < ranks; i++) {
        /* New nodes of 'higher' rank than all nodes generated till now.  */
        int new_nodes = MIN_PER_RANK
                        + (rand() % (MAX_PER_RANK - MIN_PER_RANK + 1));

        /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
        for (j = 0; j < nodes; j++)
            for (k = 0; k < new_nodes; k++)
                if ((rand() % 100) < PERCENT && visited[j]) {
                    printf("  %d -> %d ;\n", j, k + nodes); /* An Edge.  */
                    visited[k+nodes] = true;
                }
        nodes += new_nodes; /* Accumulate into old node set.  */
    }
    printf("}\n");
    return 0;
}