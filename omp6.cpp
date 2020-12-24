#include <omp.h>
#include <stdio.h>
#include <time.h>

int main()
{
  __uint64_t iterationCount = 1000000;
  int sum = 0;

  int chunkSize = 10000;

  // Reducion
  double startTime = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, chunkSize) reduction(+ \
                                                                : sum)
  for (int i = 0; i < iterationCount; ++i)
  {
    sum += 1;
  }
  double endTime = omp_get_wtime();
  double timeSpent = (endTime - startTime);
  printf("Sum = %d; Reduction spent time = %f\n", sum, timeSpent);
  sum = 0;

  // Atomic
  startTime = omp_get_wtime();
#pragma omp parallel for shared(sum) schedule(dynamic, chunkSize)
  for (int i = 0; i < iterationCount; ++i)
  {
#pragma omp atomic
    sum += 1;
  }
  endTime = omp_get_wtime();
  timeSpent = endTime - startTime;
  printf("Sum = %d; Atomic spent time = %f\n", sum, timeSpent);
  sum = 0;

  // Critical
  startTime = omp_get_wtime();
#pragma omp parallel for shared(sum) schedule(dynamic, chunkSize)
  for (int i = 0; i < iterationCount; ++i)
  {
#pragma omp critical
    sum += 1;
  }
  endTime = omp_get_wtime();
  timeSpent = endTime - startTime;
  printf("Sum = %d; Critical spent time = %f\n", sum, timeSpent);
  sum = 0;

  // Lock
  omp_lock_t lock;
  omp_init_lock(&lock);
  startTime = omp_get_wtime();
#pragma omp parallel for shared(sum) schedule(dynamic, chunkSize)
  for (int i = 0; i < iterationCount; ++i)
  {
    omp_set_lock(&lock);
    sum += 1;
    omp_unset_lock(&lock);
  }
  endTime = omp_get_wtime();
  timeSpent = endTime - startTime;
  printf("Sum = %d; Lock spent time = %f\n", sum, timeSpent);
  sum = 0;
}
