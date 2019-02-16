#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

float** omp_kmeans(int, float**, int, int, int, float, int*, int);
float** seq_kmeans(float**, int, int, int, float, int*, int*);

float euclid_dist_2(int, float*, float*);
int find_nearest_cluster(int, int, float*, float**);

float** file_read(char*, int*, int*);
int file_write(char*, int, int, int, float**, int*);
