#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

double wtime(void)
{
    double now_time;
    struct timeval  etstart;

    now_time = ((double)etstart.tv_sec) +              /* in seconds */
               ((double)etstart.tv_usec) / 1000000.0;  /* in microseconds */
    return now_time;
}
