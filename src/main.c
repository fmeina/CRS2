#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "point.h"
#include "function.h"
#include "crs2.h"

#define X_CNT 2
#define POINTS_CNT (1000 * (X_CNT + 1))

void printTime(struct timeval *start, struct timeval *stop)
{
	long time = 1000000 * (stop->tv_sec - start->tv_sec) + stop->tv_usec - start->tv_usec;

	printf("\nOpenMP execution time=%ld microseconds\n", time);
}

int main(int argc, char **argv)
{
	struct timeval start, stop;
	int thread;
	int threadnum = omp_get_max_threads();

	printf("Number of max threads: %d\r\n", threadnum);
	gettimeofday(&start, NULL);
	// run your computations here (including OpenMP stuff)

	point_t *points;
	point_t result;
	const function_t *p_optimized_function = sum_func_get();

	if (2 == argc)
	{
		if (0 == strcmp("engval", argv[1]))
		{
			puts("Performing Engvall's function optimization.");
			p_optimized_function = engval_func_get();
		} 
		else if (0 == strcmp("sum", argv[1]))
		{
			puts("Performing sum function optimization.");
			p_optimized_function = sum_func_get();
		}
		else
		{
			fputs("Incorrect argument\r\n", stderr);
			return -1;
		}
	}
	else
	{
		puts("Defaulting to sum function optimization");
	}

	point_init(&result, X_CNT);

	crs2_init();
	crs2_rand_points_generate(&points, POINTS_CNT, X_CNT, p_optimized_function);
	crs2_optimize(points, POINTS_CNT, X_CNT, p_optimized_function, &result);
	crs2_points_free(points, POINTS_CNT);

	fputs("Results: ", stdout);
	point_print(&result, X_CNT);

	point_free(&result);

	gettimeofday(&stop, NULL);
	printTime(&start, &stop);
}
