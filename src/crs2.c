#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "point.h"
#include "function.h"
#include "crs2.h"

#define CRS2_ITERATIONS_MAX 100000
#define CRS2_STOP_CONDITION 0.01

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
        
        points_arr[i].y = 0;

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

void crs2_optimize(point_t *const points_arr, const int points_cnt, const int x_cnt, const function_t* p_f, point_t *const result)
{
    bool constraints_ok = true;
    int cntr = 0;
    point_t *p_worst_point;
    point_t *p_best_point;
    point_t *const R = malloc((x_cnt + 1) * sizeof(point_t));
    point_t centroid;
    point_t next_trial_point;
    point_init(&centroid, x_cnt);
    point_init(&next_trial_point, x_cnt);

    /* step 1 - evaluate random points results */
    for (size_t i = 0; i < points_cnt; i++)
    {
        points_arr[i].y = p_f->p_func(points_arr[i].x_arr, x_cnt);
        point_print(&points_arr[i], x_cnt);
    }

    p_worst_point = &points_arr[0];
    p_best_point = &points_arr[0];

    /* perform optimization */
    for (cntr = 0; cntr < CRS2_ITERATIONS_MAX; ++cntr)
    {
        /* step 2 - find worst and best point */
        for (size_t i = 0; i < points_cnt; i++)
        {
            point_t *const p = &points_arr[i];

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
            constraints_ok = true;
            // memset(centroid.x_arr, 0, x_cnt * sizeof(double));

            /* step 3 - select next trial point */
            R[0] = *p_best_point;

            for (size_t i = 1; i < (x_cnt + 1);)
            {
                int idx = rand() % points_cnt;

                if (p_best_point != &points_arr[idx])
                {
                    R[i] = points_arr[idx];
                    ++i;
                }
            }

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
            
            if (next_trial_point.y < p_worst_point->y)
            {
                memcpy(p_worst_point->x_arr, next_trial_point.x_arr, x_cnt * sizeof(double));
                p_worst_point->y = next_trial_point.y;
                break;
            }
        }

        printf("Iteration %06d:", cntr);
        fputs("\tbest: ", stdout);
        point_print(p_best_point, x_cnt);
        fputs("\t\t\tworst: ", stdout);
        point_print(p_worst_point, x_cnt);

        double eps1 = fabs(1 - (p_worst_point->y / p_best_point->y));
        double eps2 = fabs((p_worst_point->y - p_best_point->y));

        printf("\t\t\tepsilon1 = %f\r\n", eps1);
        printf("\t\t\tepsilon2 = %f\r\n", eps2);
        fflush(stdout);


        if (fmin(eps1, eps2) < CRS2_STOP_CONDITION)
        {
            puts("Stop condition hit");
            break;
        }
    }

    memcpy(result->x_arr, p_best_point->x_arr, x_cnt * sizeof(double));
    result->y = p_best_point->y;

    point_free(&next_trial_point);
    point_free(&centroid);
    free(R);
}
