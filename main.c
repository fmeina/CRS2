#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>

#include "src/functions/testFunctions.h"

#define r_a 4000000
#define r_b 5000000

int main(int argc,char **argv) {
    // basic array of doubles
    double inputArr[] = {2,8,9,4,3,8,9,6};
    int arrayCount = (sizeof(inputArr) / sizeof(inputArr[0]));
    int option;
    double result;
    double changedArray[arrayCount + 1];

    printf("Choose algorithm: ");
    scanf("%d", &option);

    switch (option) {
        case 1:
            printf("You've chosen Powell singular function");

            // start array with 1

            changedArray[0] = 0;
            for (int i = 1; i < arrayCount+1; ++i) {
                changedArray[i] = inputArr[i-1];
            }

            result = powellSingularFunction(changedArray, arrayCount);
            break;
        case 2:
            printf("You've chosen Woods function");

            // start array with 1
            changedArray[0] = 0;
            for (int i = 1; i < arrayCount+1; ++i) {
                changedArray[i] = inputArr[i-1];
            }

            result = woodsFunction(changedArray, arrayCount);
            break;
        case 3:
            printf("You've chosen Arrowhead function.");
            result = arrowheadFunction(inputArr, arrayCount);
            break;
        default:
            printf("Wrong option");
    }

    printf("\n Result is: %f", result);
    return 0;
}
