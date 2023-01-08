#include "functionsChecks.h"

#include <stdbool.h>

bool isDivisibleByFour(int arrayCount)
{
    int div = arrayCount % 4;

    if (div == 0)
    {
        return true;
    }
    return false;
}