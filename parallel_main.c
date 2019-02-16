#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "kmeans.h"

int main(int argc, char **argv)
{
    int is_perform_atomic;

    int numCoords, numObjs;
    int *membership;    /* [numObjs] */
    float **objects;       /* [numObjs][numCoords] data objects */
    float **clusters;      /* [numClusters][numCoords] cluster center */
    double timing, clustering_timing;
    float threshold = 0.001;
    char *filename = argv[1];
    int numClusters = atoi(argv[2]);
    int nthreads = atoi(argv[3]);

    is_perform_atomic = 0;

    omp_set_num_threads(nthreads);

    objects = file_read(filename, &numObjs, &numCoords);
    if (objects == NULL) exit(1);

    /* start the core computation -------------------------------------------*/
    /* membership: the cluster id for each data object */
    clustering_timing = omp_get_wtime();
    membership = (int*) malloc(numObjs * sizeof(int));
    clusters = omp_kmeans(is_perform_atomic, objects, numCoords, numObjs,
                          numClusters, threshold, membership);
    free(objects[0]);
    free(objects);

    timing = omp_get_wtime();
    clustering_timing = timing - clustering_timing;

    file_write(filename, numClusters, numObjs, numCoords, clusters, membership);

    free(membership);
    free(clusters[0]);
    free(clusters);

    printf("\n___Parallel OpenMP Kmeans___");
    if (is_perform_atomic)
        printf(" using atomic pragma ***\n");
    else
        printf(" using array reduction ***\n");

    printf("Number of threads = %d\n", nthreads);
    printf("Input file:     %s\n", filename);
    printf("numObjs       = %d\n", numObjs);
    printf("numCoords     = %d\n", numCoords);
    printf("numClusters   = %d\n", numClusters);
    printf("threshold     = %.4f\n", threshold);
    printf("Computation timing = %10.4f sec\n", clustering_timing);
    return(0);
}
