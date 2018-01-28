all:
	gcc dining-philosophers.c -lpthread -o dining-philosophers
	gcc dining-philosophers-omp.c -fopenmp -o dining-philosophers-omp

clean:
	rm dining-philosophers
	rm dining-philosophers-omp
