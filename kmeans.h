
float** omp_kmeans(int, float**, int, int, int, float, int*);
float** seq_kmeans(float**, int, int, int, float, int*, int*);

float** file_read(char*, int*, int*);
int     file_write(char*, int, int, int, float**, int*);

double  wtime(void);
