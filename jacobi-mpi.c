#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <mpi.h>
#include "util.h"

void Jacobi_step(double *u_k, int N, int size, double h);
double norm(double *u_k, int N, double h);

int main(int argc, char **argv) {

  int N, passes, i;
  int rank, size;
  double h, message;
  double *u;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 3) {
    fprintf(stderr, "Function needs vector size and number of passes as input arguments!\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  N = atoi(argv[1]);
  passes = atoi(argv[2]);


  if (N % size != 0) {
    fprintf(stderr, "Vector size not divisible!\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  u = (double *) malloc( ((N / size) + 2) * sizeof(double));
  h = 1.0 / (N + 1);

// $$$$$$$ TO DO NORM $$$$$$$$$
// compute the initial norm
//  init = norm(u, N, h);
//  eps = init / 10e6;

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for(i=0; i< passes; ++i) {

    Jacobi_step(u, N, size, h);    

    if(rank == 0) {
      message = u[(N/size)];
      //printf("Processor %d sending %f to %d\n", rank, message, rank+1);
      MPI_Send(&message, 1, MPI_DOUBLE, rank + 1, 999, MPI_COMM_WORLD);

      MPI_Recv(&message, 1, MPI_DOUBLE, rank + 1, 999, MPI_COMM_WORLD, &status);
      u[(N/size) + 1] = message;
      //printf("Processor %d recieved %f from %d\n", rank, message, rank+1);
    }
    else if(rank == size - 1) {
      MPI_Recv(&message, 1, MPI_DOUBLE, rank - 1, 999, MPI_COMM_WORLD, &status);
      u[0] = message;
      //printf("Processor %d recieved %f from %d\n", rank, message, rank-1);

      message = u[1];
      //printf("Processor %d sending %f to %d\n", rank, message, rank-1);
      MPI_Send(&message, 1, MPI_DOUBLE, rank - 1, 999, MPI_COMM_WORLD);
    }
    else {
      MPI_Recv(&message, 1, MPI_DOUBLE, rank - 1, 999, MPI_COMM_WORLD, &status);
      u[0] = message;
      //printf("Processor %d recieved %f from %d\n", rank, message, rank-1);

      message = u[1];
      //printf("Processor %d sending %f to %d\n", rank, message, rank-1);
      MPI_Send(&message, 1, MPI_DOUBLE, rank - 1, 999, MPI_COMM_WORLD);

      message = u[(N/size)];
      //printf("Processor %d sending %f to %d\n", rank, message, rank+1);
      MPI_Send(&message, 1, MPI_DOUBLE, rank + 1, 999, MPI_COMM_WORLD);

      MPI_Recv(&message, 1, MPI_DOUBLE, rank + 1, 999, MPI_COMM_WORLD, &status);
      u[(N/size) + 1] = message;
      //printf("Processor %d recieved %f from %d\n", rank, message, rank+1);
    }

  }
  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1, time2);

  // print first part of the solution to verify correctness
  if(rank == 0) {
    //    for(i = 1; i < N / size + 2; i += size)
    //      printf("%1.4f %f\n", 1.0/ ((double)N) * i, u[i]);
    printf("Iterations: %d\n", passes);
    printf("Grid points: %d\n", N);
    printf("Number of cores %d\n", size);
    printf("Time elaspsed: %f seconds\n", elapsed);    
  }


  
  //   if (i % 10 == 0) {
  //     res = norm(u, N, h);
  //     printf("%d: %f\n", i, res);
  //     if (res < eps) break;
  //   }
 
  // }
  // for(i=0; i<N; i++)
  //   printf("%f\n", u[i]);

  free(u);
  return 0;
  }


// this may require mpi reduce
double norm(double *u_k, int N, double h) {

  double total = 0.0;
  int i;

  total += abs( h * (2.0 * u_k[0] - u_k[1]) - 1.0 );
  for(i = 1; i < N-1; ++i) 
    total += abs( h * (2.0 * u_k[i] - u_k[i-1] - u_k[i+1]) - 1.0 );
  total += abs( h * (2.0 * u_k[N-1] - u_k[N-2]) - 1.0 );

  return total;
}

void Jacobi_step(double *u, int N, int size, double h) {

  double temp[2] = {0.0, 0.0};
  double h2 = h*h;
  int i;
  
  for ( i = 1; i < (N/size)+1; ++i ) {
    temp[i%2] = (h2 / 2.0) * ( 1 - (-1.0 / h2) * u[i-1] \
			       - (-1.0 / h2) * u[i+1] );
    u[i-1] = temp[(i+1)%2];
  }
  u[i-1] = temp[(i+1)%2];
}

