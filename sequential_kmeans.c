#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAX_LINE_LEN 256

float compute_distance(int dimentions, float *point1, float *point2)
{
    int i;
    float distance = 0.0;
    for (i=0; i<dimentions; i++)
        distance += (point1[i]-point2[i]) * (point1[i]-point2[i]);
    return distance;
}

int closest_cluster(int clusters_number, int coordinates_number, float  *observation, float **clusters)
{
    int class = 0, i;
    float d, min_d;
    min_d = compute_distance(coordinates_number, observation, clusters[0]);
    for (i=1; i<clusters_number; i++) {
        d = compute_distance(coordinates_number, observation, clusters[i]);
        if (d < min_d) {
            min_d = d;
            class = i;
        }
    }
    return class;
}

/* return an array of cluster centers of size [clusters_number][coordinates_number]       */
float** sequential_kmeans(float **observations,      /* in: [observ_num][coordinates_number] */
                   int     coordinates_number,    /* no. features */
                   int     observ_num,      /* no. observations */
                   int     clusters_number,  /* no. clusters */
                   float   threshold,    /* % observations change membership */
                   int    *membership)   /* out: [observ_num] */
{
    int i, j, index, loop=0;
    int *new_cluster_size; /* [clusters_number]: no. observations assigned in each
                                new cluster */
    float diff;          /* % of observations change their clusters */
    float **clusters;       /* out: [clusters_number][coordinates_number] */
    float **new_clusters;    /* [clusters_number][coordinates_number] */

    /* allocate a 2D space for returning variable clusters[] (coordinates
       of cluster centers) */
    clusters = (float**) malloc(clusters_number * sizeof(float*));
    clusters[0] = (float*)  malloc(clusters_number * coordinates_number * sizeof(float));
    for (i=1; i<clusters_number; i++)
        clusters[i] = clusters[i-1] + coordinates_number;

    /* pick first clusters_number elements of observations[] as initial cluster centers*/
    for (i=0; i<clusters_number; i++)
        for (j=0; j<coordinates_number; j++)
            clusters[i][j] = observations[i][j];

    /* initialize membership[] */
    for (i=0; i<observ_num; i++) membership[i] = -1;

    /* need to initialize new_cluster_size and new_clusters[0] to all 0 */
    new_cluster_size = (int*) calloc(clusters_number, sizeof(int));

    new_clusters = (float**) malloc(clusters_number *  sizeof(float*));
    new_clusters[0] = (float*)  calloc(clusters_number * coordinates_number, sizeof(float));
    for (i=1; i<clusters_number; i++)
        new_clusters[i] = new_clusters[i-1] + coordinates_number;

    do {
        diff = 0.0;
        for (i=0; i<observ_num; i++) {
            /* find the array index of nestest cluster center */
            index = closest_cluster(clusters_number, coordinates_number, observations[i],
                                         clusters);

            /* if membership changes, increase diff by 1 */
            if (membership[i] != index) diff += 1.0;

            /* assign the membership to object i */
            membership[i] = index;

            /* update new cluster centers : sum of observations located within */
            new_cluster_size[index]++;
            for (j=0; j<coordinates_number; j++)
                new_clusters[index][j] += observations[i][j];
        }
        /* average the sum and replace old cluster centers with new_clusters */
        for (i=0; i<clusters_number; i++) {
            for (j=0; j<coordinates_number; j++) {
                if (new_cluster_size[i] > 0)
                    clusters[i][j] = new_clusters[i][j] / new_cluster_size[i];
                new_clusters[i][j] = 0.0;   /* set back to 0 */
            }
            new_cluster_size[i] = 0;   /* set back to 0 */
        }

        diff /= observ_num;
    } while (diff > threshold && loop++ < 500);

    free(new_clusters[0]);
    free(new_clusters);
    free(new_cluster_size);

    return clusters;
}

float** readf(char *filename, int *observ_num, int *coordinates_number)
{
    float **observations;
    int i, j, size;
    FILE *file;
    char *row;
    int rowLength;

    file = fopen(filename, "r");
    if(file == NULL) {
        fprintf(stderr, "Can not open file.\n");
    }

    rowLength = MAX_LINE_LEN;
    row = (char*) malloc(rowLength);

    (*observ_num) = 0;
    // find max length of row
    while (fgets(row, rowLength, file) != NULL) {
        if (strtok(row, " \t\n") != 0)
            (*observ_num)++;
    }
    rewind(file);

    //find the number of coloms of each observation
    (*coordinates_number) = 0;
    while (fgets(row, rowLength, file) != NULL) {
        if (strtok(row, " \t\n") != 0) {
            while (strtok(NULL, " ,\t\n") != NULL)
                (*coordinates_number)++;
            break;
        }
    }
    rewind(file);

    size = (*observ_num) * (*coordinates_number);
    observations = (float**) malloc((*observ_num) * sizeof(float*));
    observations[0] = (float*) malloc(size * sizeof(float));
    for (i=1; i<(*observ_num); i++)
        observations[i] = observations[i-1] + (*coordinates_number);

    i = 0;
    while (fgets(row, rowLength, file) != NULL) {
        if (strtok(row, " \t\n") == NULL) continue;
        for (j=0; j<(*coordinates_number); j++)
            observations[i][j] = atof(strtok(NULL, " ,\t\n"));
        i++;
    }
    fclose(file);
    free(row);
    return observations;
}

int writef(int clusters_number, int observ_num, int coordinates_number,
               float **clusters, int *result)
{
    FILE *file;
    int i, j;
    char filename[1024];
    strcpy(filename, "data/centroids.txt");
    printf("Write coordinates of centroids to \"%s\"\n", filename);
    file = fopen(filename, "w");
    for (i=0; i<clusters_number; i++) {
        fprintf(file, "%d ", i);
        for (j=0; j<coordinates_number; j++)
            fprintf(file, "%f ", clusters[i][j]);
        fprintf(file, "\n");
    }
    fclose(file);

    strcpy(filename, "data/clustering.txt");
    printf("Write allocation of %d oservations to \"%s\"\n", observ_num, filename);
    file = fopen(filename, "w");
    for (i=0; i<observ_num; i++)
        fprintf(file, "%d %d\n", i, result[i]);
    fclose(file);
    return 1;
}

int main(int argc, char **argv)
{
    int coordinates_number, observ_num;
    int *membership;    /* [observ_num] */
    float **observations;       /* [observ_num][coordinates_number] data observations */
    float **clusters;      /* [clusters_number][coordinates_number] cluster center */
    double timing;
    clock_t end, start;
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
    clusters = sequential_kmeans(observations, coordinates_number, observ_num, clusters_number, threshold, membership);
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
    printf("Computation time = %10.4f sec\n", timing);
    return(0);
}
