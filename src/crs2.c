#include <stdio.h>
#define _CRT_RAND_S
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <mpi.h>
#include "point.h"
#include "function.h"
#include "crs2.h"
#include "common.h"

#define CRS2_ITERATIONS_MAX 1000000
#define CRS2_STOP_CONDITION 0.001

extern bool verbose;

void crs2_init(void)
{
    srand(time(0));
}

void crs2_rand_points_generate(point_t **const p_points, const int points_cnt, const int x_cnt, const function_t* p_f)
{
    *p_points = malloc(points_cnt * sizeof(point_t));
    point_t *const points_arr = *p_points;
    
    /* generate random points arguments */
    for (size_t i = 0; i < points_cnt; i++)
    {
        point_init(&points_arr[i], x_cnt);

        for (size_t j = 0; j < x_cnt; j++)
        {
            points_arr[i].x_arr[j] = (rand() % (p_f->domain_max - p_f->domain_min + 1)) + p_f->domain_min;

        }

        points_arr[i].y = p_f->p_func(points_arr[i].x_arr, x_cnt);
        if (true == verbose)
        {
            point_print(&points_arr[i], x_cnt);
        }
    }
}

void crs2_points_free(point_t *const points_arr, const int points_cnt)
{
    for (size_t i = 0; i < points_cnt; i++)
    {
        point_free(&points_arr[i]);
    }
    
    free(points_arr);
}

static void stop_rq_get(int *p_stop_rq)
{
	MPI_Iprobe(MPI_ANY_SOURCE, CRS_TAG_STOP_RQ, MPI_COMM_WORLD, p_stop_rq, MPI_STATUS_IGNORE);
}

static void stop_rq_set()
{
	int world_rank;
	int world_size;
	int stop_rq = true;

	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	for (int i = 0; i < world_size; ++i)
	{
		if (world_rank != i)
		{
			MPI_Send(&stop_rq, 1, MPI_C_BOOL, i, CRS_TAG_STOP_RQ, MPI_COMM_WORLD);
		}
	}
}

void result_snd(const point_t* const p_result, const int x_cnt)
{
	MPI_Send(p_result->x_arr, x_cnt, MPI_DOUBLE, CRS_RANK_MAIN, CRS_TAG_RESULT_X, MPI_COMM_WORLD);
	MPI_Send(&p_result->y, 1, MPI_DOUBLE, CRS_RANK_MAIN, CRS_TAG_RESULT_Y, MPI_COMM_WORLD);
}

int crs2_optimize(point_t *const points_arr, const int points_cnt, const int x_cnt, const function_t* p_f)
{
    bool constraints_ok = true;
    int cntr = 0;
	int stop_rq = 0;
	int world_rank;
    point_t *p_worst_point;
    point_t *p_best_point;
    point_t *const R = malloc((x_cnt + 1) * sizeof(point_t));
    point_t centroid;
    point_t next_trial_point;
	point_t result;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    unsigned int rand_seed = time(NULL) ^ world_rank;

    point_init(&centroid, x_cnt);
    point_init(&next_trial_point, x_cnt);
	point_init(&result, x_cnt);

    p_worst_point = &points_arr[0];
    p_best_point = &points_arr[0];

    /* perform optimization */
    for (cntr = 0; cntr < CRS2_ITERATIONS_MAX; ++cntr)
    {
		/* step 2 - find worst and best point */
		for (size_t i = 0; i < points_cnt; i++)
		{
			point_t* const p = &points_arr[i];

			if (p->y < p_best_point->y)
			{
				p_best_point = p;
			}
			else if (p->y > p_worst_point->y)
			{
				p_worst_point = p;
			}
			else
			{
				/* do nothing */
			}
		}


        while (true)
        {
			stop_rq_get(&stop_rq);

            if (true == stop_rq)
            {
                cntr = -1;
                break;
            }

            constraints_ok = true;
            // memset(centroid.x_arr, 0, x_cnt * sizeof(double));

            /* step 3 - select next trial point */
            // #pragma omp critical
            // {
                R[0] = *p_best_point;

                for (size_t i = 1; i < (x_cnt + 1);)
                {
                    unsigned int idx;
#ifdef __linux__
                    idx = rand_r(&rand_seed) % points_cnt;
#elif _WIN32
                    rand_s(&idx);
                    idx %= points_cnt;
#else
#error unsupported system
#endif

                    if (p_best_point != &points_arr[idx])
                    {
                        R[i] = points_arr[idx];
                        ++i;
                    }
                }
            // }

            for (size_t i = 0; i < x_cnt; i++)
            {
                for (size_t j = 0; j < x_cnt; j++)
                {
                    centroid.x_arr[i] += R[j].x_arr[i];
                }

                centroid.x_arr[i] /= x_cnt;
                next_trial_point.x_arr[i] = 2 * centroid.x_arr[i] - R[x_cnt].x_arr[i];

                if ((next_trial_point.x_arr[i] < p_f->domain_min) || (next_trial_point.x_arr[i] > p_f->domain_max))
                {
                    constraints_ok = false;
                }
            }

            /* Step 4 - check constraints */
            if (false == constraints_ok)
            {
                continue;
            }
            
            next_trial_point.y = p_f->p_func(next_trial_point.x_arr, x_cnt);
            
            bool stop = false;
            #pragma omp critical
            {
                if (next_trial_point.y < p_worst_point->y)
                {
                    memcpy(p_worst_point->x_arr, next_trial_point.x_arr, x_cnt * sizeof(double));
                    p_worst_point->y = next_trial_point.y;
                    stop = true;
                }
            }

            if (true == stop)
            {
                break;
            }
        }

		stop_rq_get(&stop_rq);

        if (true == stop_rq)
        {
			cntr = -1;
			break;
        }

        if (true == verbose)
        {   
            printf("Thread %d ", omp_get_thread_num());
            printf("Iteration %06d:", cntr);
            fputs("\tbest: ", stdout);
            point_print(p_best_point, x_cnt);
            fputs("\t\t\t\tworst: ", stdout);
            point_print(p_worst_point, x_cnt);
        }

        // putchar('.');

        double eps1 = fabs(1 - (p_worst_point->y / p_best_point->y));
        double eps2 = fabs((p_worst_point->y - p_best_point->y));

        // printf("\t\t\tepsilon1 = %f\r\n", eps1);
        // printf("\t\t\tepsilon2 = %f\r\n", eps2);
        fflush(stdout);


        if (fmin(eps1, eps2) < CRS2_STOP_CONDITION)
        {
            if (true == verbose)
            {
                printf("Worker done \r\n");
            }

			stop_rq_set();
			memcpy(result.x_arr, p_best_point->x_arr, x_cnt * sizeof(double));
			result.y = p_best_point->y;
			result_snd(&result, x_cnt);
            break;
        }
    }

	point_free(&result);
    point_free(&next_trial_point);
    point_free(&centroid);
    free(R);

    return cntr;
}

