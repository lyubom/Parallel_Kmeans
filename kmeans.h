#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

float** parallel_kmeans(int, float**, int, int, int, float, int*, int);
float** sequential_kmeans(float**, int, int, int, float, int*, int*);

float compute_distance(int, float*, float*);
int closest_cluster(int, int, float*, float**);

float** readf(char*, int*, int*);
int writef(int, int, int, float**, int*);
