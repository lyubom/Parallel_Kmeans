#include "kmeans.h"

int main(int argc, char **argv)
{
    int is_perform_atomic;

    int coordinates_number, observ_num;
    int *membership;    /* [observ_num] */
    float **observations;       /* [observ_num][coordinates_number] data observations */
    float **clusters;      /* [clusters_number][coordinates_number] cluster center */
    double end, start;
    float threshold = 0.001;
    char *filename = argv[1];
    int clusters_number = atoi(argv[2]);
    int threads_number = atoi(argv[3]);

    is_perform_atomic = 0;

    omp_set_num_threads(threads_number);

    observations = readf(filename, &observ_num, &coordinates_number);
    if (observations == NULL) exit(1);

    /* start the core computation -------------------------------------------*/
    /* membership: the cluster id for each data object */
    start = omp_get_wtime();
    membership = (int*) malloc(observ_num * sizeof(int));
    clusters = parallel_kmeans(is_perform_atomic, observations, coordinates_number, observ_num,
                          clusters_number, threshold, membership, threads_number);
    free(observations[0]);
    free(observations);
    end = omp_get_wtime();
    start = end - start;

    writef(clusters_number, observ_num, coordinates_number, clusters, membership);

    free(membership);
    free(clusters[0]);
    free(clusters);

    printf("\n___Parallel OpenMP Kmeans___");
    if (is_perform_atomic)
        printf(" using atomic pragma ***\n");
    else
        printf(" using array reduction ***\n");

    printf("Number of threads\t= %d\n", threads_number);
    printf("Input file:\t%s\n", filename);
    printf("observation number\t= %d\n", observ_num);
    printf("coordinates_number\t= %d\n", coordinates_number);
    printf("clusters_number\t= %d\n", clusters_number);
    printf("threshold\t= %.4f\n", threshold);
    printf("Computation timing = %10.4f sec\n", start);
    return(0);
}
