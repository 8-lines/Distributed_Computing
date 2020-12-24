#include <mpi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <ctime> 
  
// size of array 
#define n 100
  
int a[n][n]; 
  
// Temporary array for slave process 
int a2[n]; 
  
int main(int argc, char* argv[]) 
{ 
  
    int pid, np, row_nb,
        rows_per_process, 
        n_elements_recieved,
        requests_sent;
    double time_start;
    // np -> no. of processes 
    // pid -> process id 
  
    MPI_Status status; 
    MPI_Request request;
  
    // Creation of parallel processes 
    MPI_Init(&argc, &argv); 
  
    // find out process ID, 
    // and how many processes were started 
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); 
    MPI_Comm_size(MPI_COMM_WORLD, &np); 
    //printf("Process online [%d/%d]\n", pid, np);
  
    // master process 
    if (pid == 0) { 
        int index, i;
        rows_per_process = n / np; 

        srand(static_cast<unsigned int>(time(0)));
        time_start = MPI_Wtime();
        //printf("Generated matrix is:\n");
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                a[i][j] = rand() % 201 - 100;
                //printf("%d ", a[i][j]);
            }
            //printf("\n");
        }
        //printf("\n");

        int maxmin = -999999;
  
        rows_per_process = n/np;
        int left = n % np;
        requests_sent = n - rows_per_process;
        if (left != 0)
        {
            left = left - 1;
            requests_sent = requests_sent - 1;
        }

        for (i = 1; i < np; i++) 
        {
            int temp = rows_per_process;
            if (left > 0)
            {
                temp = temp + 1;
                left = left - 1;
            }

            MPI_Isend(&temp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request); 

        }

        for (i = 0; i < n; i++) 
        { 
            // distributes the portion of array 
            // to child processes to calculate 
            // their partial mins 
            if (i % np != 0)
            {
                MPI_Isend(&a[i], 
                        n, 
                        MPI_INT, (i % np), 0, 
                        MPI_COMM_WORLD, &request); 
            }
            else
            {
                int min = a[i][0];
                //printf("Master curr min is: %d\n", min);
                //calculate min by master processor
                for (int j = 0; j < n; j++)
                {
                    if (a[i][j] < min)
                    min = a[i][j];
                }
                //printf("Curr min of proc [0] row [%d] is: %d\n", i, min);
                if (min > maxmin)
                    maxmin = min;
            }
        }
   
  
        // collects mins from other processes 
        int tmp; 
        for (i = 0; i < requests_sent; i++) { 
            MPI_Recv(&tmp, 1, MPI_INT, 
                     MPI_ANY_SOURCE, 0, 
                     MPI_COMM_WORLD, 
                     &status); 
            int sender = status.MPI_SOURCE; 

            if (tmp > maxmin)
                maxmin = tmp; 
        } 
  
        // prints the final maxmin of matrix 
        //printf("Maxmin of matrix is : %d\n\n", maxmin); 
    } 
    // slave processes 
    else { 
        //recieve the number of rows
        MPI_Recv(&row_nb, 1, 
                 MPI_INT, 0, 0, 
                 MPI_COMM_WORLD, 
                 &status); 

        for (int k = 0; k < row_nb; k++)
        {
            // stores the received array segment 
            // in local array a2 
            MPI_Recv(&a2, n, 
                    MPI_INT, 0, 0, 
                    MPI_COMM_WORLD, 
                    &status); 
    
            // calculates its partial min 
            int curr_min = a2[0]; 
            for (int i = 0; i < n; i++) 
                if (a2[i] < curr_min)
                    curr_min = a2[i]; 
    
            //printf("Curr min of proc [%d] row [%d] is: %d\n", pid, (k*np + pid), curr_min);
            // sends the partial min to the root process 
            MPI_Isend(&curr_min, 1, MPI_INT, 
                    0, 0, MPI_COMM_WORLD, &request); 
        }

        
    } 

    double delta = MPI_Wtime() - time_start;
    if (pid == 0)
        printf("Total time: %f\n", delta);
    // cleans up all MPI state before exit of process 
    MPI_Finalize(); 
  
    return 0; 
} 
