#include <mpi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <ctime> 
  
// size of array 
#define n 1000 
  
int a[n]; 
  
// Temporary array for slave process 
int a2[n]; 
  
int main(int argc, char* argv[]) 
{ 
  
    int pid, np, 
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
    //printf("Process online [%d/%d]\n", pid, np);

    double time_start = MPI_Wtime();
  
    // master process 
    if (pid == 0) { 
        int index, i; 
        elements_per_process = n / np; 

        srand(static_cast<unsigned int>(time(0)));
        //printf("Generated array is:\n");
        for (int i = 0; i < n; i++)
        {
            a[i] = rand() % 1001 - 500;
            //printf("%d ", a[i]);
        }
        //printf("\n");
  
        // check if more than 1 processes are run 
        if (np > 1) { 
            // distributes the portion of array 
            // to child processes to calculate 
            // their partial mins 
            for (i = 1; i < np - 1; i++) { 
                index = i * elements_per_process; 
  
                MPI_Send(&elements_per_process, 
                         1, MPI_INT, i, 0, 
                         MPI_COMM_WORLD); 

                MPI_Send(&a[index], 
                         elements_per_process, 
                         MPI_INT, i, 0, 
                         MPI_COMM_WORLD); 
            } 
  
            // last process adds remaining elements 
            index = i * elements_per_process; 
            int elements_left = n - index; 
  
            MPI_Send(&elements_left, 
                     1, MPI_INT, 
                     i, 0, 
                     MPI_COMM_WORLD); 

            MPI_Send(&a[index], 
                     elements_left, 
                     MPI_INT, i, 0, 
                     MPI_COMM_WORLD); 
        } 
  
        // master process add its own sub array 
        int min = a[0]; 
        printf("Start min is: %d\n", min);
        for (i = 0; i < elements_per_process; i++) 
            if (a[i] < min)
                min = a[i]; 
  
        // collects mins from other processes 
        int tmp; 
        for (i = 1; i < np; i++) { 
            MPI_Recv(&tmp, 1, MPI_INT, 
                     MPI_ANY_SOURCE, 0, 
                     MPI_COMM_WORLD, 
                     &status); 
            int sender = status.MPI_SOURCE; 

            //printf("Curr min of proc [%d] is: %d\n", sender, tmp);

            if (tmp < min)
                min = tmp; 
        } 
  
        // prints the final min of array 
        printf("Min of array is : %d\n", min); 
    } 
    // slave processes 
    else { 
        MPI_Recv(&n_elements_recieved, 
                 1, MPI_INT, 0, 0, 
                 MPI_COMM_WORLD, 
                 &status); 
  
        // stores the received array segment 
        // in local array a2 
        MPI_Recv(&a2, n_elements_recieved, 
                 MPI_INT, 0, 0, 
                 MPI_COMM_WORLD, 
                 &status); 
  
        // calculates its partial min 
        int curr_min = a2[0]; 
        for (int i = 0; i < n_elements_recieved; i++) 
            if (a2[i] < curr_min)
                curr_min = a2[i]; 
  
        // sends the partial min to the root process 
        MPI_Send(&curr_min, 1, MPI_INT, 
                 0, 0, MPI_COMM_WORLD); 
    } 
  
    double delta = MPI_Wtime() - time_start;
    if (pid == 0)
        printf("Total time: %f\n", delta);

    // cleans up all MPI state before exit of process 
    MPI_Finalize(); 
  
    return 0; 
} 
