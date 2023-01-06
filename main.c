#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>

#define r_a 4000000
#define r_b 5000000

bool isPrimeNumber(long nbr){
	for (int i = 2; i <=sqrt(nbr); i++)
    {
		if (nbr % i == 0)
        {
			return 0;
		}
	}
	return 1;
}

bool PrimeNumberSum(long nbr, bool printInfo) {
	for (long x = 3; x < nbr; x += 2)
    {
		if (isPrimeNumber(x) && isPrimeNumber(nbr - x))
        {
            if (printInfo)
            {
                printf("\n %ld can be created with %ld + %ld \n", nbr, x, nbr-x);
            }

			return 1;
        }
    }
	return 0;
}

int hipGoldbach(long a, long b) {
    bool printInfo = true;
	for (long i = a; i <= b; i++)
    {
		if (i <= 2 || i%2!=0)
        {
			continue;
		}
		if (!PrimeNumberSum(i, printInfo))
        {
			return 0;
		}
        printInfo = false;
	}

	return 1;
}


void printTime(struct timeval *start,struct timeval *stop) {
  long time=1000000*(stop->tv_sec-start->tv_sec)+stop->tv_usec-start->tv_usec;

  printf("\nOpenMP execution time=%ld microseconds\n",time);

}

int main(int argc,char **argv) {
  struct timeval start,stop;
  int result = 1;
  int thread;
  int r_start;
  int r_end;
  int threadnum = omp_get_max_threads();

  printf("Number of max threads: %d", threadnum);

  int range = (r_b - r_a)/threadnum;
  gettimeofday(&start,NULL);
 // run your computations here (including OpenMP stuff)

#pragma omp parallel private(thread, r_start, r_end) reduction(* : result)
{
  thread = omp_get_thread_num();
  r_start = r_a + (thread*range);
  r_end = r_start + range;
  result = hipGoldbach(r_start, r_end);
}
  printf("\n Goldbach hypothesis %s \n", result ? "confirmed": "denied" );
  gettimeofday(&stop,NULL);

  printTime(&start,&stop);

}
