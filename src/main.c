#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include "point.h"
#include "function.h"
#include "crs2.h"

#define X_CNT            2
#define POINTS_CNT       (10 * (X_CNT + 1))


void printTime(struct timeval *start,struct timeval *stop) {
  long time=1000000*(stop->tv_sec-start->tv_sec)+stop->tv_usec-start->tv_usec;

  printf("\nOpenMP execution time=%ld microseconds\n",time);

}

int main(int argc,char **argv) {
  struct timeval start,stop;
  int thread;
  int threadnum = omp_get_max_threads();

  printf("Number of max threads: %d\r\n", threadnum);
  gettimeofday(&start,NULL);
 // run your computations here (including OpenMP stuff)

  point_t *points;
  point_t result;

  point_init(&result, X_CNT);
  
  crs2_init();
  crs2_rand_points_generate(&points, POINTS_CNT, X_CNT, engval_func_get());
  crs2_optimize(points, POINTS_CNT, X_CNT, engval_func_get(), &result);  
  crs2_points_free(points, POINTS_CNT);

  fputs("Results: ", stdout);
  point_print(&result, X_CNT);
  
  point_free(&result);

  gettimeofday(&stop,NULL);
  printTime(&start,&stop);
}
