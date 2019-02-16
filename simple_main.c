#include "kmeans.h"

int main(int argc, char **argv)
{
    int coordinates_number, observ_num;
    int *membership;    /* [observ_num] */
    float **observations;       /* [observ_num][coordinates_number] data observations */
    float **clusters;      /* [clusters_number][coordinates_number] cluster center */
    double timing;
    clock_t end, start;
    int iterations;
    float threshold = 0.001;
    char *filename = argv[1];
    int clusters_number = atoi(argv[2]);

    /* read data points from file ------------------------------------------*/
    observations = readf(filename, &observ_num, &coordinates_number);
    if (observations == NULL) exit(1);

    /* start the timer for the core computation -----------------------------*/
    /* membership: the cluster id for each data object */
    start = clock();
    membership = (int*) malloc(observ_num * sizeof(int));
    clusters = sequential_kmeans(observations, coordinates_number, observ_num, clusters_number, threshold,
                          membership, &iterations);
    free(observations[0]);
    free(observations);
    end = clock();
    timing = ((double)(end - start ))/ CLOCKS_PER_SEC;


    writef(clusters_number, observ_num, coordinates_number, clusters, membership);

    free(membership);
    free(clusters[0]);
    free(clusters);

    printf("\n___Sequential Kmeans___\n");
    printf("Input file:\t%s\n", filename);
    printf("observation number\t= %d\n", observ_num);
    printf("coordinatesnumber\t= %d\n", coordinates_number);
    printf("clusters numbers\t= %d\n", clusters_number);
    printf("threshold\t= %.4f\n", threshold);
    printf("iterations\t= %d\n", iterations);
    printf("Computation time = %10.4f sec\n", timing);
    return(0);
}
