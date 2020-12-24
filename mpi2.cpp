#include <mpi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <ctime> 
  
// size of array 
#define n 15 
  
int a[n];
int b[n];

// Temporary arrays for slave process 
int a2[n];
int b2[n];
  
int main(int argc, char* argv[]) 
{ 
  
    int pid, np, i,
        elements_per_process, 
        n_elements_recieved; 
    // np -> no. of processes 
    // pid -> process id 
  
    MPI_Status status; 
    // Creation of parallel processes 
    MPI_Init(&argc, &argv); 
  
    // find out process ID, 
    // and how many processes were started 
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); 
    MPI_Comm_size(MPI_COMM_WORLD, &np); 

    double time_start = MPI_Wtime();
  
    // master process 
    if (pid == 0) 
    { 
        int index, i; 
        elements_per_process = n / np; 

        // Generates an array
        srand(static_cast<unsigned int>(time(0)));
        printf("Generated array is:\n");
        for (int i = 0; i < n; i++)
        {
            a[i] = rand() % 1001 - 500;
            b[i] = rand() % 1001 - 500;
            printf("%d ", a[i]);
        }
       printf("\n"); 

        // Just for output of B
        for (int i = 0; i < n; i++)
            printf("%d ", b[i]);

        printf("\n"); 
    }

    MPI_Bcast(&elements_per_process, 1,
            MPI_INT, 0, 
            MPI_COMM_WORLD); 

    MPI_Scatter(&a, elements_per_process, 
            MPI_INT, &a2, elements_per_process, MPI_INT, 0, 
            MPI_COMM_WORLD);

    MPI_Scatter(&b, elements_per_process, 
            MPI_INT, &b2, elements_per_process, MPI_INT, 0, 
            MPI_COMM_WORLD);  

    if (pid == 0) 
    { 
        // master process adds its own sub array 
        long long prod = 0; 

        // Calculate prod
        for (i = 0; i < elements_per_process; i++) 
            prod += a[i] * b[i];

        // and processes the last elements
        for (i = elements_per_process*np; i < n; i++) 
            prod += a[i] * b[i];

        printf("Curr prod of proc [0] is: %d\n", prod);

        // collects prods from other processes 
        int tmp; 
        for (i = 1; i < np; i++) { 
            MPI_Recv(&tmp, 1, MPI_INT, 
                     MPI_ANY_SOURCE, 0, 
                     MPI_COMM_WORLD, 
                     &status); 
            int sender = status.MPI_SOURCE; 

            printf("Curr prod of proc [%d] is: %d\n", sender, tmp);

            prod += tmp;
        } 
  
        // prints the final prod of array 
        printf("Prod of array is : %d\n", prod); 
    } 

    // slave processes 
    if (pid != 0)  { 
    
        // calculates its partial prod 
        int curr_prod = 0; 
        for (int i = 0; i < elements_per_process; i++) 
            curr_prod += a2[i]*b2[i];
  
        // sends the partial prod to the root process 
        MPI_Send(&curr_prod, 1, MPI_INT, 
                 0, 0, MPI_COMM_WORLD); 
    } 
  
    double delta = MPI_Wtime() - time_start;
    if (pid == 0)
        printf("Total time: %f\n", delta);

    // cleans up all MPI state before exit of process 
    MPI_Finalize(); 
  
    return 0; 
} 
