#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
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
#include <mpi.h>
#include "point.h"
#include "function.h"
#include "crs2.h"
#include "common.h"

bool verbose = false;

void printTime(struct timeval* start, struct timeval* stop)
{
	long time = 1000000 * (stop->tv_sec - start->tv_sec) + stop->tv_usec - start->tv_usec;

	printf("\nExecution time = %ld microseconds\n", time);
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

static void run_mode_ditributed_exec(int points_cnt, int x_cnt, function_t const* p_f);

int main(int argc, char** argv)
{
	struct timeval start, stop;
	int c;
	opterr = 0;
	const function_t* p_optimized_function;
	int x_cnt = 0;
	int points_cnt = 0;
	point_t result;

	while (-1 != (c = getopt(argc, argv, "hN:n:v")))
	{
		switch (c)
		{
		case 'h':
			help_print();
			return 0;
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
//		x_cnt = 2;
		return -1;
	}

	if (0 == points_cnt)
	{
		fputs("No N specified. Aborting\r\n", stderr);
//		points_cnt = 2000;
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
//		p_optimized_function = arrowhead_function_get();
		exit(-1);
	}

	printf("N = %d\r\nn = %d\r\n", points_cnt, x_cnt);
	gettimeofday(&start, NULL);
	point_init(&result, x_cnt);
	run_mode_ditributed_exec(points_cnt, x_cnt,	p_optimized_function);
	gettimeofday(&stop, NULL);
	printTime(&start, &stop);
}

static void run_mode_ditributed_exec(int points_cnt, int x_cnt, function_t const* p_f)
{
	int world_size;
	int world_rank;
	char proc_name[MPI_MAX_PROCESSOR_NAME];
	int proc_name_len;
	point_t *p_points = NULL;
	point_t result;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Get_processor_name(proc_name, &proc_name_len);
	point_init(&result, x_cnt);

	if (CRS_RANK_MAIN == world_rank)
	{
		/* main rank generates N random points */
		crs2_init();
		puts("main worker generating random points");
		crs2_rand_points_generate(&p_points, points_cnt, x_cnt, p_f);
	}
	else
	{
		/* other workers only allocates space for them */
		p_points = malloc(points_cnt * sizeof(point_t));
		for (int i = 0; i < points_cnt; ++i)
		{
			point_init(&p_points[i], x_cnt);
		}
	}

	/* broadcast N random points from CRS_RANK_MAIN to all others */
	for (int i = 0; i < points_cnt; ++i)
	{
		MPI_Bcast(p_points[i].x_arr, x_cnt, MPI_DOUBLE, CRS_RANK_MAIN, MPI_COMM_WORLD);
		MPI_Bcast(&p_points[i].y, 1, MPI_DOUBLE, CRS_RANK_MAIN, MPI_COMM_WORLD);
	}

	/* probably unnecessary but sync here to be sure */
	MPI_Barrier(MPI_COMM_WORLD);

	/* perform optimization */
	printf("Worker %d out of %d starting\n", world_rank, world_size);
	crs2_optimize(p_points, points_cnt, x_cnt, p_f);

	if (CRS_RANK_MAIN == world_rank)
	{
		MPI_Recv(&result.y, 1, MPI_DOUBLE, MPI_ANY_SOURCE, CRS_TAG_RESULT_Y, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(result.x_arr, x_cnt, MPI_DOUBLE, MPI_ANY_SOURCE, CRS_TAG_RESULT_X, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		fputs("Result: ", stdout);
		point_print(&result, x_cnt);
	}

//	MPI_Recv(result, sizeof(point_t), MPI_BYTE, MPI_ANY_SOURCE, CRS_TAG_RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);



//	MPI_Barrier(MPI_COMM_WORLD);
	point_free(&result);
	crs2_points_free(p_points, points_cnt);
	MPI_Finalize();
}

#pragma clang diagnostic pop