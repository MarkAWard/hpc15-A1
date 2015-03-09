/* 
 * send a single integer around a ring to all processors
 * start with 0 and add rank to integer then pass the message
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include "util.h"

int main( int argc, char *argv[])
{
  int rank, tag, origin, destination, size, message;
  MPI_Status status;

  int N = atoi(argv[1]);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  tag = 99;
  message = 0;

  timestamp_type time1, time2;
  get_timestamp(&time1);

  int i;
  for(i = 0; i < N; i++) {

    destination = (rank + 1) % size;
    origin = rank - 1;

    if(rank == 0) {
      origin = size - 1;
      message += rank;
      MPI_Send(&message, 1, MPI_INT, destination, tag, MPI_COMM_WORLD);
      MPI_Recv(&message, 1, MPI_INT, origin,      tag, MPI_COMM_WORLD, &status);      
    }
    else {
      MPI_Recv(&message, 1, MPI_INT, origin,      tag, MPI_COMM_WORLD, &status);    
      message += rank;
      MPI_Send(&message, 1, MPI_INT, destination, tag, MPI_COMM_WORLD);
    }
  }

  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1, time2);

  if(rank == 0) {
    printf("Loops: %d, Sum: %d\n", i, message);
    printf("Total messages passed: %d\n", i * size);
    printf("Time elaspsed: %f seconds\n", elapsed);
    printf("Estimated latency per message: %f seconds\n", elapsed / (double)(i * size));
  }

  MPI_Finalize();
  return 0;
}
