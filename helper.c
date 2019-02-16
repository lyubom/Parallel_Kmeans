#include "kmeans.h"

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
