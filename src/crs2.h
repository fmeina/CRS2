#ifndef CRS2_H
#define CRS2_H

void crs2_init(void);

void crs2_rand_points_generate(point_t **const p_points, const int points_cnt, const int x_cnt, const function_t* p_f);

int crs2_optimize(point_t *const points_arr, const int points_cnt, const int x_cnt, const function_t* p_f);

void crs2_points_free(point_t *const points_arr, const int points_cnt);

#endif // CRS2_H