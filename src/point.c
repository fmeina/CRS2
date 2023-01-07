#include <stdlib.h>
#include <stdio.h>
#include "point.h"

int point_init(point_t *const p_point, const int x_cnt)
{
    int err = -1;

    if (NULL != p_point)
    {
        p_point->x_arr = calloc(x_cnt, sizeof(double));
        
        if (NULL != p_point->x_arr)
        {
            p_point->y = 0.0;
            err = 0;
        }
    }

    return err;
}

void point_free(point_t *const p_point)
{
    if (NULL != p_point)
    {
        free(p_point->x_arr);
    }
}

void point_print(point_t *const p_point, const int x_cnt)
{
    if (NULL != p_point)
    {
        fputs("X = { ", stdout);

        for (size_t i = 0; i < x_cnt; i++)
        {
            printf("%-10.4f", p_point->x_arr[i]);

            if ((x_cnt - 1) > i)
            {
                fputs(", ", stdout);
            }
        }

        printf(" }\ty = %-10.4f\r\n", p_point->y);
    }
    else
    {
        fputs("ERR: point is NULL\r\n", stderr);
    }
}
