EXECUTABLES = int_ring int_ring_array jacobi
COMPILER = mpicc 
FLAGS = -O3 -Wall

all: $(EXECUTABLES)

int_ring: int_ring.c 
	$(COMPILER) $(FLAGS) $^ -o $@

int_ring_array: int_ring_array.c 
	$(COMPILER) $(FLAGS) $^ -o $@

jacobi: jacobi.c
	$(COMPILER) $(FLAGS) $^ -o $@

clean:
	rm -rf $(EXECUTABLES) *~
