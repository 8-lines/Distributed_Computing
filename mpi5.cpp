#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
using namespace std;

#define N 100
#define Nmb 1000

int main (int argc, char *argv[]) {
    int start, size, tag = 0;
    char message[N];
    for (int i = 0; i < N; i++)
        message[i] = (char)(i % 10) + 48;

    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &start);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    double time_start = MPI_Wtime();
    int bufsize = 3 * N;
    char* buf = (char *)malloc( bufsize );
    MPI_Buffer_attach( buf, bufsize );

    for (int k = 0; k < Nmb; k++) 
    {
        
        int end = (start + 1) % size;
        MPI_Send(message, N, MPI_CHAR, end, tag, MPI_COMM_WORLD);
        //MPI_Ssend(message, N, MPI_CHAR, end, tag, MPI_COMM_WORLD);
        //MPI_Rsend(message, N, MPI_CHAR, end, tag, MPI_COMM_WORLD);
        //MPI_Bsend(message, N, MPI_CHAR, end, tag, MPI_COMM_WORLD);
        end = (size + start - 1) % size;

        MPI_Recv(&message, N, MPI_CHAR, end, tag, MPI_COMM_WORLD, &status);

        //printf("Process № %d, message: %s \n", start, message);
    }

    double delta = MPI_Wtime() - time_start;
    if (start == 0)
        printf("Total time: %f \n", delta);

    MPI_Buffer_detach( &buf, &bufsize );
    MPI_Finalize();

    return 0;
}