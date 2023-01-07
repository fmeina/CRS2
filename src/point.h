#ifndef POINT_H
#define POINT_H

typedef struct
{
    double *x_arr;
    double y; 
} point_t;

int point_init(point_t *const p_point, const int x_cnt);

void point_free(point_t *const p_point);

void point_print(point_t *const p_point, const int x_cnt);

#endif /* POINT_H */