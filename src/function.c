#include <stdlib.h>
#include <math.h>
#include "function.h"

static double engval_func(const double *x, const int n);
static double sum_func(const double *x, const int n);

const function_t *engval_func_get(void)
{
    static const function_t result = 
    {
        .domain_min = -2000.0,
        .domain_max = 2000.0,
        .p_func = engval_func
    };

    return &result;
}

const function_t *sum_func_get(void)
{
    static const function_t result = 
    {
        .domain_min = -2000.0,
        .domain_max = 2000.0,
        .p_func = sum_func
    };

    return &result;
}

static double engval_func(const double *x, const int n)
{
    double result = 0.0;

    for (size_t i = 2-1; i < n; i++)
    {
        result += pow(pow(x[i-1], 2) + pow(x[i], 2), 2) - 4 * x[i-1] + 3;
    }
    
    return result;
}

static double sum_func(const double *x, const int n)
{
    double result = 0.0;

    for (size_t i = 0; i < n; i++)
    {
        result += x[i];
    }
    
    return result;
};