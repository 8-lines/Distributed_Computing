#include <omp.h>
#include <stdio.h>
#include <stdlib.h> 
#include <ctime> 

int main()
{
  int N = 10000;
  int threads = 4;
  printf("\n\n\nProgram start!\n");
  //printf("Input the size of vectors: ");
  //scanf("%d", &N);

  // Fill vector
  int* a = new int[N];

  srand(static_cast<unsigned int>(time(0)));

  //printf("Input the number of treads: ");
  //scanf("%d", &threads);

  for (int j = 0; j < 4; j++)
  {   
    int* a = new int[N];
    for (int i = 0; i < N; i++)
    {
        a[i] = rand() % 11 - 5;
    }

    int min_el = 99999;

    double time_start = omp_get_wtime();

    for (int i = 0; i < N; ++i)
        if (min_el > a[i])
		min_el = a[i];

    double delta = (omp_get_wtime() - time_start);

    printf("\nUsual time: %f\n", delta);

    for (int k = 0; k < 4; k++)
    {
        min_el = 99999;
        omp_set_num_threads(threads);
        int chunkSize = N / threads;
        printf("N = %d, Threads = %d\n", N, threads);
        time_start = omp_get_wtime();

        #pragma omp parallel shared(a, b, N)
        {
                #pragma omp for schedule(dynamic, chunkSize) reduction (min:min_el)
                    for (int i = 0; i < N; ++i)
                        if (a[i] < min_el)
					min_el = a[i];
        }

        delta = (omp_get_wtime() - time_start);
        printf("Parallel time: %f\n", delta);

        //double acc = delta1/delta2;
        //printf("Acceleration = %f\n", acc);

        threads *= 2;
    }
    N *= 10;
    threads = 4;
    delete a;
    delete b;
  }
}
