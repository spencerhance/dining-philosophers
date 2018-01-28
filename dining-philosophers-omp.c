/* Dining philosopher problem
 *
 * Each thread is one philospher
 *
 * gcc dining-philosophers.c -lomp
 *
 * ./dining-philosophers <num_philosophers>
 */


#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>

#define NUM_MEALS 10000

// Get time in milliseconds
double CLOCK()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec * 1e-6);
}


// Function for each philosopher thread
void diningPhilosophers(int num_philosophers, omp_lock_t *forks)
{
    int tid = omp_get_thread_num();
    int left_fork_index = tid;
    int right_fork_index = (tid + 1) % num_philosophers;

    int i = NUM_MEALS;
   
    // Iterate until all meals have been eaten
    while (i > 0)
    {
        // Sleep a random amount of time between 0-10 microseconds 
        usleep(rand() % 11);

        // Lock forks
        #pragma omp critical
        {
            omp_set_lock(&forks[left_fork_index]);
            omp_set_lock(&forks[right_fork_index]);
        }

        // Eat
        printf("Philosopher %i is eating, %i meals left\n", tid, i);
        
        // Unlock forks
        omp_unset_lock(&forks[left_fork_index]);
        omp_unset_lock(&forks[right_fork_index]);

        // One less meal 
        i--;
    }
    
    // Print when finished
    printf("Philosopher %i is done\n", tid);
}


int main(int argc, char* argv[])
{
    // Usage
    char* usage = "Usage: ./dining-philosophers <num_philosophers>";
    
    // Make sure we have enough args
    if (argc < 2)
    {
        printf("%s\n", usage);
        return 1;
    }

    // Time vars
    double start_time, end_time, total_time;

    // argv[1] is num philosophers / num threads
    int num_philosophers = atoi(argv[1]);

    // Sanity check, need at least 3 philosophers
    assert(num_philosophers > 2);

    // Seed rand
    srand(time(NULL));

    // Initialize array of forks
    omp_lock_t *forks = malloc(sizeof(omp_lock_t) * num_philosophers);
    for (int i = 0; i < num_philosophers; i++)
        omp_init_lock(&forks[i]);

    // Start time
    start_time = CLOCK();
    
    # pragma omp parallel for
    for (int i = 0; i < num_philosophers; i++)
        diningPhilosophers(num_philosophers, forks);
    
    // Get end time
    end_time = CLOCK();

    // Print time
    total_time  = end_time - start_time;
    printf("Time: %f milliseconds\n", total_time);

    // Cleanup
    free(forks);

    return 0;
}
