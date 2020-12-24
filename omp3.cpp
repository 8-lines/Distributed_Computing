#include <omp.h>
#include <stdio.h>

float f(float x)
{
  return x*x + x + 5;
}


int main()
{
  // Init params
  int N = 1000000;
  float a = 0;
  float b = 1000;
  float h = (b - a) / N;

  // Init sum
  float sum = 0;

  double startTime = omp_get_wtime();

  omp_set_num_threads(3);
#pragma omp parallel shared(N, a, b, h)
  {
#pragma omp for schedule(guided) reduction(+: sum)
    for (int i = 0; i < N; ++i)
    {
      float x = i*h;
      sum += h * f(x);
    }
  }


  double endTime = omp_get_wtime();
  double timeSpentParallel = (endTime - startTime);

  sum = 0;

  startTime = omp_get_wtime();

  for (int i = 0; i < N; ++i)
  {
    float x = i*h;
    sum += h * f(x);
  }
    
  printf("Integral value = %f\n", sum);

  endTime = omp_get_wtime();
  double timeSpentNotparallel = (endTime - startTime);

  double acc = timeSpentNotparallel/timeSpentParallel;

  printf("Acceleration = %f\n", acc);
}
