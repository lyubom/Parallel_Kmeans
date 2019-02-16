#include "kmeans.h"

/* return an array of cluster centers of size [clusters_number][coordinates_number]       */
float** parallel_kmeans(int is_perform_atomic, /* in: */
                   float **observations,           /* in: [observ_num][coordinates_number] */
                   int coordinates_number,         /* no. coordinates */
                   int observ_num,           /* no. observations */
                   int clusters_number,       /* no. clusters */
                   float threshold,         /* % observations change membership */
                   int *membership,        /* out: [observ_num] */
                   int threads_number)
{

    int i, j, k, index, loop=0;
    int *new_cluster_size; /* [clusters_number]: no. observations assigned in each                                 new cluster */
    float diff;          /* % of observations change their clusters */
    float **clusters;       /* out: [clusters_number][coordinates_number] */
    float **new_clusters;    /* [clusters_number][coordinates_number] */

    int **local_new_cluster_size; /* [threads_number][clusters_number] */
    float ***local_new_clusters;    /* [threads_number][clusters_number][coordinates_number] */


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

    new_clusters = (float**) malloc(clusters_number * sizeof(float*));
    new_clusters[0] = (float*)  calloc(clusters_number * coordinates_number, sizeof(float));
    for (i=1; i<clusters_number; i++)
        new_clusters[i] = new_clusters[i-1] + coordinates_number;

    if (!is_perform_atomic) {
        /* each thread calculates new centers using a private space,
           then thread 0 does an array reduction on them. This approach
           should be faster */
        local_new_cluster_size = (int**) malloc(threads_number * sizeof(int*));
        local_new_cluster_size[0] = (int*)  calloc(threads_number*clusters_number,
                                                 sizeof(int));
        for (i=1; i<threads_number; i++)
            local_new_cluster_size[i] = local_new_cluster_size[i-1]+clusters_number;

        /* local_new_clusters is a 3D array */
        local_new_clusters =(float***)malloc(threads_number * sizeof(float**));
        local_new_clusters[0] =(float**) malloc(threads_number * clusters_number * sizeof(float*));
        for (i=1; i<threads_number; i++)
            local_new_clusters[i] = local_new_clusters[i-1] + clusters_number;
        for (i=0; i<threads_number; i++) {
            for (j=0; j<clusters_number; j++) {
                local_new_clusters[i][j] = (float*)calloc(coordinates_number, sizeof(float));
            }
        }
    }

    do {
        diff = 0.0;

        if (is_perform_atomic) {
            #pragma omp parallel for \
                    private(i,j,index) \
                    firstprivate(observ_num,clusters_number,coordinates_number) \
                    shared(observations,clusters,membership,new_clusters,new_cluster_size) \
                    schedule(static) \
                    reduction(+:diff)
            for (i=0; i<observ_num; i++) {
                /* find the array index of nestest cluster center */
                index = closest_cluster(clusters_number, coordinates_number, observations[i],
                                             clusters);

                /* if membership changes, increase diff by 1 */
                if (membership[i] != index) diff += 1.0;

                /* assign the membership to object i */
                membership[i] = index;

                /* update new cluster centers : sum of observations located within */
                #pragma omp atomic
                new_cluster_size[index]++;
                for (j=0; j<coordinates_number; j++)
                    #pragma omp atomic
                    new_clusters[index][j] += observations[i][j];
            }
        }
        else {
            #pragma omp parallel shared(observations,clusters,membership,local_new_clusters,local_new_cluster_size)
            {
                int tid = omp_get_thread_num();
                #pragma omp for \
                            private(i,j,index) \
                            firstprivate(observ_num,clusters_number,coordinates_number) \
                            schedule(static) \
                            reduction(+:diff)
                for (i=0; i<observ_num; i++) {
                    /* find the array index of nestest cluster center */
                    index = closest_cluster(clusters_number, coordinates_number,
                                                 observations[i], clusters);

                    /* if membership changes, increase diff by 1 */
                    if (membership[i] != index) diff += 1.0;

                    /* assign the membership to object i */
                    membership[i] = index;

                    /* update new cluster centers : sum of all observations located
                       within (average will be performed later) */
                    local_new_cluster_size[tid][index]++;
                    for (j=0; j<coordinates_number; j++)
                        local_new_clusters[tid][index][j] += observations[i][j];
                }
            } /* end of #pragma omp parallel */

            /* let the main thread perform the array reduction */
            for (i=0; i<clusters_number; i++) {
                for (j=0; j<threads_number; j++) {
                    new_cluster_size[i] += local_new_cluster_size[j][i];
                    local_new_cluster_size[j][i] = 0.0;
                    for (k=0; k<coordinates_number; k++) {
                        new_clusters[i][k] += local_new_clusters[j][i][k];
                        local_new_clusters[j][i][k] = 0.0;
                    }
                }
            }
        }

        /* average the sum and replace old cluster centers with new_clusters */
        for (i=0; i<clusters_number; i++) {
            for (j=0; j<coordinates_number; j++) {
                if (new_cluster_size[i] > 1)
                    clusters[i][j] = new_clusters[i][j] / new_cluster_size[i];
                new_clusters[i][j] = 0.0;   /* set back to 0 */
            }
            new_cluster_size[i] = 0;   /* set back to 0 */
        }

        diff /= observ_num;
    } while (diff > threshold && loop++ < 500);

    if (!is_perform_atomic) {
        free(local_new_cluster_size[0]);
        free(local_new_cluster_size);

        for (i=0; i<threads_number; i++)
            for (j=0; j<clusters_number; j++)
                free(local_new_clusters[i][j]);
        free(local_new_clusters[0]);
        free(local_new_clusters);
    }
    free(new_clusters[0]);
    free(new_clusters);
    free(new_cluster_size);

    return clusters;
}
