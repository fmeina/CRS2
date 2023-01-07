#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "testFunctions.h"
#include "functionsChecks.h"

double powellSingularFunction(double inputArr[], int arrayCount)
{
    bool validInput = isDivisibleByFour(arrayCount);

    if (validInput) {
        int numberOfIterations = arrayCount / 4;

        double sum = 0;
        double term1 = 0;
        double term2 = 0;
        double term3 = 0;
        double term4 = 0;

        for (int i = 1; i <= numberOfIterations; i++) {
            term1 = pow((inputArr[4 * i - 3] + 10 * inputArr[4 * i - 2]), 2);
            term2 = 5 * pow((inputArr[4 * i - 1] - inputArr[4 * i]), 2);
            term3 = pow((inputArr[4 * i - 2] - 2 * inputArr[4 * i - 1]), 4);
            term4 = 10 * pow((inputArr[4 * i - 3] - inputArr[4 * i]), 4);

            sum = sum + term1 + term2 + term3 + term4;
        }

        return sum;
    }

    printf("Wrong input");

    return 0;
}

double woodsFunction(double inputArr[], int arrayCount)
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

        for (int i = 1; i <= numberOfIterations; i++) {
            term1 = 100 * pow((inputArr[4 * i - 2] - pow((inputArr[4 * i - 3]),2)), 2);
            term2 = pow((1 - inputArr[4 * i - 3]), 2);
            term3 = 90 * pow((inputArr[4 * i] - pow((inputArr[4 * i - 1]),2)), 2);
            term4 = pow((1 - inputArr[4 * i - 1]), 2);
            term5 = 10 * pow((inputArr[4 * i - 2] + inputArr[4 * i] - 2), 2);
            term6 = 0.1 * pow((inputArr[4 * i - 2] - inputArr[4 * i]), 2);

            sum = sum + term1 + term2 + term3 + term4 + term5 + term6;
        }

        return sum;
    }

    printf("Wrong input");

    return 0;
}

double arrowheadFunction(double inputArr[], int arrayCount)
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
