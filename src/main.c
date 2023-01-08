#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>
#include "point.h"
#include "function.h"
#include "crs2.h"

bool verbose = false;

void printTime(struct timeval *start, struct timeval *stop)
{
	long time = 1000000 * (stop->tv_sec - start->tv_sec) + stop->tv_usec - start->tv_usec;

	printf("\nOpenMP execution time = %ld microseconds\n", time);
}

void help_print(void)
{
	puts("Usage: CRS2 [-hpNnv] alg_name");
	puts("alg_name (mandatory):");
	puts("\tengval");
	puts("\tpowell");
	puts("\twoods");
	puts("\tarrowhead");
	puts("Flags:");
	puts("\t-h - (optional) print this help");
	puts("\t-p - (optional) enable parallel execution (threads number defined by OMP_NUM_THREADS env variable)");
	puts("\t-N [int] - (mandatory) sets random points number");
	puts("\t-n [int] - (mandatory) sets optimized function arguments count");
	puts("\t-v - (optional) verbose output");
}

int main(int argc, char **argv)
{
	struct timeval start, stop;
	int thread;
	// int threadnum = omp_get_max_threads();
	bool parallel_exec_flag = false;
	int c;
	opterr = 0;
	const function_t *p_optimized_function;
	int x_cnt = 0;
	int points_cnt = 0;
	int iterations;
	point_t *points;
	point_t result;
	bool stop_flag = false;

	while (-1 != (c = getopt(argc, argv, "hpN:n:v")))
	{
		switch (c)
		{
		case 'h':
			help_print();
			return 0;
			break;

		case 'p':
			parallel_exec_flag = true;
			break;
		
		case 'N':
			points_cnt = strtol(optarg, NULL, 0);
			break;
		
		case 'n':
			x_cnt = strtol(optarg, NULL, 0);
			break;

		case 'v':
			verbose = true;
			break;

		case '?':
			fputs("Unknown flag or argument\r\n", stderr);
			help_print();
			return -1;
			break;
		
		case ':':
			printf("-%c without argument\n", optopt);
			help_print();
			return -1;
			break;

		default:
			abort();
			break;
		}
	}

	if (0 == x_cnt)
	{
		fputs("No n specified. Aborting\r\n", stderr);
		return -1;
	}

	if (0 == points_cnt)
	{
		fputs("No N specified. Aborting\r\n", stderr);
		return -1;
	}

	if (optind < argc)
	{
		if (0 == strcmp("engval", argv[optind]))
		{
			puts("Performing Engvall's function optimization.");
			p_optimized_function = engval_func_get();
		}
		else if (0 == strcmp("powell", argv[optind]))
		{
			puts("Performing Powell singular function optimization.");
			p_optimized_function = powell_singular_function_get();
		}
		else if (0 == strcmp("woods", argv[optind]))
		{
			puts("Performing woods function optimization.");
			p_optimized_function = woods_function_get();
		}
		else if (0 == strcmp("arrowhead", argv[optind]))
		{
			puts("Performing arrowhead function optimization.");
			p_optimized_function = arrowhead_function_get();
		}
		else
		{
			fprintf(stderr, "Incorrect algorithm: %s\r\n", argv[optind]);
			return -1;
		}
	}
	else
	{
		fputs("No algorithm given. Aborting.\r\n", stderr);
		exit(-1);
	}
	
	printf("N = %d\r\nn = %d\r\n", points_cnt, x_cnt);

	gettimeofday(&start, NULL);
	// run your computations here (including OpenMP stuff)

	point_init(&result, x_cnt);
	crs2_init();
	crs2_rand_points_generate(&points, points_cnt, x_cnt, p_optimized_function);

	if (true == parallel_exec_flag)
	{
		printf("Parralel execution using %d threads begin\r\n", omp_get_max_threads());

		#pragma omp parallel shared(stop_flag, points, p_optimized_function, result)
		{
			printf("Thread %d begin\r\n", omp_get_thread_num());
			if (-1 != crs2_optimize(points, points_cnt, x_cnt, p_optimized_function, &result, &stop_flag))
			{
				printf("Thread %d found solution\r\n", omp_get_thread_num());
			}
			
			printf("Thread %d end\r\n", omp_get_thread_num());
		}
	}
	else
	{
		puts("Sequential execution begin");
		crs2_optimize(points, points_cnt, x_cnt, p_optimized_function, &result, &stop_flag);

	}

	fputs("Result: ", stdout);
	point_print(&result, x_cnt);


	crs2_points_free(points, points_cnt);
	point_free(&result);

	gettimeofday(&stop, NULL);
	printTime(&start, &stop);
}
