#ifndef FUNCTION_H
#define FUNCTION_H

typedef struct
{
    int domain_min;
    int domain_max;
    double (*p_func)(const double*const, const int n);
} function_t;

const function_t *engval_func_get(void);

const function_t *sum_func_get(void);

#endif // FUNCTION_H