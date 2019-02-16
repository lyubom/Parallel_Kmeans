#include <stdio.h>
#include <stdlib.h>
#include "kmeans.h"

int main(int argc, char **argv)
{
    int numCoords, numObjs;
    int *membership;    /* [numObjs] */
    float **objects;       /* [numObjs][numCoords] data objects */
    float **clusters;      /* [numClusters][numCoords] cluster center */
    double timing, clustering_timing;
    int loop_iterations;
    float threshold = 0.001;
    char *filename = argv[1];
    int numClusters = atoi(argv[2]);

    /* read data points from file ------------------------------------------*/
    objects = file_read(filename, &numObjs, &numCoords);
    if (objects == NULL) exit(1);

    /* start the timer for the core computation -----------------------------*/
    /* membership: the cluster id for each data object */
    clustering_timing = wtime();
    membership = (int*) malloc(numObjs * sizeof(int));
    clusters = seq_kmeans(objects, numCoords, numObjs, numClusters, threshold,
                          membership, &loop_iterations);
    free(objects[0]);
    free(objects);
    timing = wtime();
    clustering_timing = timing - clustering_timing;

    file_write(filename, numClusters, numObjs, numCoords, clusters, membership);

    free(membership);
    free(clusters[0]);
    free(clusters);

    printf("\n___Sequential Kmeans___\n");
    printf("Input file:     %s\n", filename);
    printf("numObjs       = %d\n", numObjs);
    printf("numCoords     = %d\n", numCoords);
    printf("numClusters   = %d\n", numClusters);
    printf("threshold     = %.4f\n", threshold);
    printf("Loop iterations    = %d\n", loop_iterations);
    printf("Computation time = %10.4f sec\n", clustering_timing);
    return(0);
}
