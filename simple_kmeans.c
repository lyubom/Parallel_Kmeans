#include "kmeans.h"

/* return an array of cluster centers of size [clusters_number][coordinates_number]       */
float** sequential_kmeans(float **observations,      /* in: [observ_num][coordinates_number] */
                   int     coordinates_number,    /* no. features */
                   int     observ_num,      /* no. observations */
                   int     clusters_number,  /* no. clusters */
                   float   threshold,    /* % observations change membership */
                   int    *membership,   /* out: [observ_num] */
                   int    *iterations)
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

    *iterations = loop + 1;

    free(new_clusters[0]);
    free(new_clusters);
    free(new_cluster_size);

    return clusters;
}
