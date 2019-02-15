#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* getopt() */

#include <omp.h>
#include "kmeans.h"

/*---< usage() >------------------------------------------------------------*/
static void usage(char *argv0, float threshold) {
    char *help =
        "Usage: %s [switches] -i filename -n num_clusters\n"
        "       -i filename    : file containing data to be clustered\n"
        "       -n num_clusters: number of clusters (K must > 1)\n"
        "       -t threshold   : threshold value (default %.4f)\n"
        "       -p nproc       : number of threads (default system allocated)\n"
        "       -a             : perform atomic OpenMP pragma (default no)\n";
    fprintf(stderr, help, argv0, threshold);
    exit(-1);
}

/*---< main() >-------------------------------------------------------------*/
int main(int argc, char **argv) {
           int     opt;
    extern char   *optarg;
    extern int     optind;
           int     i, j, nthreads;
           int      is_perform_atomic;

           int     numClusters, numCoords, numObjs;
           int    *membership;    /* [numObjs] */
           char   *filename;
           float **objects;       /* [numObjs][numCoords] data objects */
           float **clusters;      /* [numClusters][numCoords] cluster center */
           float   threshold;
           double  timing, io_timing, clustering_timing;

    /* some default values */
    nthreads = 0;
    numClusters = 0;
    threshold = 0.001;
    numClusters = 0;
    is_perform_atomic = 0;
    filename = NULL;

    while ( (opt=getopt(argc,argv,"p:i:n:t:abdo"))!= EOF) {
        switch (opt) {
            case 'i': filename=optarg;
                      break;
            case 't': threshold=atof(optarg);
                      break;
            case 'n': numClusters = atoi(optarg);
                      break;
            case 'p': nthreads = atoi(optarg);
                      break;
            case 'a': is_perform_atomic = 1;
                      break;
            default: usage(argv[0], threshold);
                      break;
        }
    }

    if (filename == 0 || numClusters <= 1) usage(argv[0], threshold);

    /* set the no. threads if specified in command line, else use all
       threads allocated by run-time system */
    if (nthreads > 0)
        omp_set_num_threads(nthreads);

    io_timing = omp_get_wtime();

    /* read data points from file ------------------------------------------*/
    objects = file_read(filename, &numObjs, &numCoords);
    if (objects == NULL) exit(1);

    timing  = omp_get_wtime();
    io_timing  = timing - io_timing;
    clustering_timing = timing;

    /* start the core computation -------------------------------------------*/
    /* membership: the cluster id for each data object */
    membership = (int*) malloc(numObjs * sizeof(int));
    assert(membership != NULL);

    clusters = omp_kmeans(is_perform_atomic, objects, numCoords, numObjs,
                          numClusters, threshold, membership);

    free(objects[0]);
    free(objects);

    timing = omp_get_wtime();
    clustering_timing = timing - clustering_timing;

    /* output: the coordinates of the cluster centres ----------------------*/
    file_write(filename, numClusters, numObjs, numCoords, clusters, membership);

    free(membership);
    free(clusters[0]);
    free(clusters);

    /*---- output performance numbers ---------------------------------------*/
    io_timing += omp_get_wtime() - timing;
    printf("\nPerforming ** Regular Kmeans (OpenMP) --");
    if (is_perform_atomic)
        printf(" using atomic pragma ***\n");
    else
        printf(" using array reduction ***\n");

    printf("Number of threads = %d\n", omp_get_max_threads());
    printf("Input file:     %s\n", filename);
    printf("numObjs       = %d\n", numObjs);
    printf("numCoords     = %d\n", numCoords);
    printf("numClusters   = %d\n", numClusters);
    printf("threshold     = %.4f\n", threshold);

    printf("I/O time           = %10.4f sec\n", io_timing);
    printf("Computation timing = %10.4f sec\n", clustering_timing);

    return(0);
}
