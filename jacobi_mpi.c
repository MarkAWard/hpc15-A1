#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <mpi.h>


void Jacobi_step(double *u_k, int N, double h);
double norm(double *u_k, int N, double h);

int main(int argc, char **argv) {

  int N, passes, i;
  int rank, size, origin, destination;
  double h, eps, res, init;
  double message;
  double *u;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc != 3) {
    fprintf(stderr, "Function needs vector size and number of passes as input arguments!\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  N = atoi(argv[1]);
  passes = atoi(argv[2])


  if (N % size != 0) {
    fprintf(stderr, "Vector size not divisible!\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  u = (double *) malloc( ((N / size) + 2) * sizeof(double));
  for(i = 0; i < (N / size) + 2; i++)
    u[i] = (i + 1) + rank * (N / size + 2);

//  h = 1.0 / (N + 1);

// $$$$$$$ TO DO NORM $$$$$$$$$
// compute the initial norm
//  init = norm(u, N, h);
//  eps = init / 10e6;

  for(i=0; i< passes; ++i) {

//    Jacobi_step(u, N, h);    

    destination = (rank + 1) % size;
    origin = rank - 1;

    // send the last element of u in the first segement 
    // to the second segment to update its first element 
    if(rank == 0) {
      message = u[(N/size) + 1]
      printf("Processor number %d sending %f\n", rank, message );
      MPI_Send(&message, 1, MPI_DOUBLE, destination, 999, MPI_COMM_WORLD);
    }
    // the last segment will only update its first element
    else if(rank == size - 1) {
      MPI_Recv(&message, 1, MPI_DOUBLE, origin, 999, MPI_COMM_WORLD);
      u[0] = message
      printf("Processor number %d recieved %f\n", rank, message );
    }
    // all middle segements will first receive and then send
    else {
      MPI_Recv(&message, 1, MPI_DOUBLE, origin, 999, MPI_COMM_WORLD);
      u[0] = message
      printf("Processor number %d recieved %f\n", rank, message );
      message = u[(N/size) + 1]
      printf("Processor number %d sending %f\n", rank, message );
      MPI_Send(&message, 1, MPI_DOUBLE, destination, 999, MPI_COMM_WORLD);
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

void Jacobi_step(double *u_k, int N, double h) {

  double temp[3] = {0.0, 0.0, 0.0};
  int i;

  temp[0] = 1.0 / (h * 2.0) * (1.0 + h * u_k[1]);
  for(i = 1; i < N-1; ++i) {
    temp[i%3] = 1.0 / (h * 2.0) * (1.0 - h * (- u_k[i-1] - u_k[i+1]));
    u_k[i-1] = temp[ i%3 == 0 ? 2 : i%3 - 1 ];
    //    printf("%d", i)
  }
  temp[(N-1)%3] = 1.0 / (h * 2.0) * (1.0 + h * u_k[i-1]);
  u_k[N-2] = temp[ (N-1)%3 == 0 ? 2 : (N-1)%3 - 1 ];
  u_k[N-1] = temp[ N%3 == 0 ? 2 : N%3 - 1 ];

}
