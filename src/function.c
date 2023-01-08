#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "function.h"
#include "functionsChecks.h"

static double engval_func(const double *x, const int n);
static double powellSingularFunction(const double inputArr[], int arrayCount);
static double woodsFunction(const double inputArr[], int arrayCount);
static double arrowheadFunction(const double inputArr[], int arrayCount);

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

const function_t *powell_singular_function_get(void)
{
    static const function_t result = 
    {
        .domain_min = -2000.0,
        .domain_max = 2000.0,
        .p_func = powellSingularFunction
    };

    return &result;
}

const function_t *woods_function_get(void)
{
    static const function_t result = 
    {
        .domain_min = -2000.0,
        .domain_max = 2000.0,
        .p_func = woodsFunction
    };

    return &result;
}

const function_t *arrowhead_function_get(void)
{
    static const function_t result = 
    {
        .domain_min = -2000.0,
        .domain_max = 2000.0,
        .p_func = arrowheadFunction
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

static double powellSingularFunction(const double inputArr[], int arrayCount)
{
    bool validInput = isDivisibleByFour(arrayCount);

    if (validInput) {
        int numberOfIterations = arrayCount / 4;

        double sum = 0;
        double term1 = 0;
        double term2 = 0;
        double term3 = 0;
        double term4 = 0;

        // first iteration outside for loop
        term1 = pow((inputArr[0] + 10 * inputArr[1]), 2);
        term2 = 5 * pow((inputArr[2] - inputArr[3]), 2);
        term3 = pow((inputArr[1] - 2 * inputArr[2]), 4);
        term4 = 10 * pow((inputArr[0] - inputArr[3]), 4);
        sum = sum + term1 + term2 + term3 + term4;

        if (numberOfIterations == 1)
        {
            return sum;
        }

        for (int i = 2; i <= numberOfIterations; i++) {
            term1 = pow((inputArr[(4 * i - 1) - 3] + 10 * inputArr[(4 * i - 1) - 2]), 2);
            term2 = 5 * pow((inputArr[(4 * i - 1) - 1] - inputArr[(4 * i - 1)]), 2);
            term3 = pow((inputArr[(4 * i - 1) - 2] - 2 * inputArr[(4 * i - 1) - 1]), 4);
            term4 = 10 * pow((inputArr[(4 * i - 1) - 3] - inputArr[(4 * i - 1)]), 4);

            sum = sum + term1 + term2 + term3 + term4;
        }

        return sum;
    }

    printf("Wrong input");

    return 0;
}

static double woodsFunction(const double inputArr[], int arrayCount)
{
    bool validInput = isDivisibleByFour(arrayCount);

    if (validInput) {
        int numberOfIterations = arrayCount / 4;

        double sum = 0;
        double term1 = 0;
        double term2 = 0;
        double term3 = 0;
        double term4 = 0;
        double term5 = 0;
        double term6 = 0;

        //first iteration outside for loop
        term1 = 100 * pow((inputArr[1] - pow((inputArr[0]),2)), 2);
        term2 = pow((1 - inputArr[0]), 2);
        term3 = 90 * pow((inputArr[3] - pow((inputArr[2]),2)), 2);
        term4 = pow((1 -inputArr[2]), 2);
        term5 = 10 * pow((inputArr[1] + inputArr[3] - 2), 2);
        term6 = 0.1 * pow((inputArr[2] - inputArr[3]), 2);

        sum = sum + term1 + term2 + term3 + term4 + term5 + term6;

        if (numberOfIterations == 1)
        {
            return sum;
        }

        for (int i = 2; i <= numberOfIterations; i++) {
            term1 = 100 * pow((inputArr[(4 * i - 1) - 2] - pow((inputArr[(4 * i - 1) - 3]),2)), 2);
            term2 = pow((1 - inputArr[(4 * i - 1) - 3]), 2);
            term3 = 90 * pow((inputArr[(4 * i - 1)] - pow((inputArr[(4 * i - 1) - 1]),2)), 2);
            term4 = pow((1 -inputArr[(4 * i - 1) - 1]), 2);
            term5 = 10 * pow((inputArr[(4 * i - 1) - 2] + inputArr[(4 * i - 1)] - 2), 2);
            term6 = 0.1 * pow((inputArr[(4 * i - 1) - 2] - inputArr[(4 * i - 1)]), 2);

            sum = sum + term1 + term2 + term3 + term4 + term5 + term6;
        }

        return sum;
    }

    printf("Wrong input");

    return 0;
}

static double arrowheadFunction(const double inputArr[], int arrayCount)
{
    double sum = 0;
    double term1 = 0;
    double term2 = 0;

    for (int i = 0; i < arrayCount - 1; i++) {
        term1 = pow((pow(inputArr[i],2) + pow(inputArr[arrayCount-1],2)),2);
        term2 = 4 * inputArr[i] + 3;


        sum = sum + term1 - term2;
    }

    return sum;

}