#include <omp.h>
#include <stdio.h>

int main()
{
  int matrixSize = 30;
  int maxInitValue = 30;

  // Fill matrix
  int matrix[matrixSize][matrixSize];
  for (int i = 0; i < matrixSize; ++i)
  {
    for (int j = 0; j < matrixSize; ++j)
    {
      // By this way the minimum elements will be on the diagonal
      if (i == j)
      {
        matrix[i][j] = i;
        continue;
      }

      matrix[i][j] = matrixSize + ((i + j) % maxInitValue);
    }
  }

  // Get chunk size
  int threadCount = omp_get_max_threads();
  int chunkSize = matrixSize / threadCount;
  printf("Chunk size = %d\n", chunkSize);

  int maxValue = 0;

  // Print matrix
  for (int i = 0; i < matrixSize; ++i)
  {
    for (int j = 0; j < matrixSize; ++j)
    {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }

#pragma omp parallel shared(matrix, maxValue)
  {
#pragma omp for schedule(dynamic, chunkSize)
    // For each row
    for (int i = 0; i < matrixSize; ++i)
    {
      int minRowValue = matrixSize + maxInitValue;

      // For each column in current row
      for (int j = 0; j < matrixSize; ++j)
      {
        if (matrix[i][j] < minRowValue)
        {
          minRowValue = matrix[i][j];
        }
      }
      printf("Min value for %d row = %d\n", i, minRowValue);

      // Check max value
      if (minRowValue > maxValue)
      {
#pragma omp critical
        if (minRowValue > maxValue)
        {
          maxValue = minRowValue;
        }
      }
    }
  }

  printf("Maxmin value = %d\n", maxValue);
}
