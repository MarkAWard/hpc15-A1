# hpc15-A1

1. MPI ring communication
  * For int_ring using 4 processors on CIMS machine linax1 and running 100,000 loops the total time was about .32 seconds on average
  * When running the same number of loops but sending messages from linax1 and then through three crunchy servers on the network the average time was about 13 seconds.
  * For a 2MB message using 4 processors on CIMS machine linax1 and running 1,000 loops the total time was about 74 seconds. Thus for sending 4,000 2MB messages the bandwidth is approximately 108 MB/s
  * Sending the same 4,000 2MB messages through a network between linax1 and three crunchy machines takes a total of 529 seconds. Therefore, the network bandwidth is approximately 15 MB/s.
  
2. Distributed memory parallel Jacobi smoother
  * The solution was independent of the number of cores used for the computation
  * I tested on CIMS crunchy1 which has 16 cores with 100,000 grid points and 100,000 iterations the runtime for different number of cores used was as follows:
    * 2 cores: 11.15 s
    * 4 cores: 6.25 s
    * 5 cores: 5.01 s
    * 8 cores: 3.74 s
    * 10 cores: 2.86 s
    * 16 cores: 2.61 s
  * A parallel Gauss-Seidel smoother is more difficult since the update for a single grid point, i, at step k+1 requires the value of the i-1 point at time k+1. This dependency makes communication difficult since the first grid point on one processor must wait until the last grid point of another processor that it needs gets updated and then transferred. 
