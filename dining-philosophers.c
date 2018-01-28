/* Dining philosopher problem
 *
 * Each thread is one philospher
 *
 * gcc dining-philosophers.c -lpthread
 *
 * ./dining-philosophers <num_philosophers>
 */


#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_MEALS 10000

// Get time in milliseconds
double CLOCK()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec * 1e-6);
}


// Info for each thread
typedef struct tinfo
{
    int id;
    pthread_mutex_t *forks;
    int left_fork_index;
    int right_fork_index;
} threadInfo;


// Function for each philosopher thread
void* philosopher(void *tinfo)
{
    threadInfo *thread_info = (threadInfo*) tinfo;

    int i = NUM_MEALS;
    while (i > 0)
    {
        // Sleep a random amount of time between 0-10 microseconds 
        usleep(rand() % 10);

        // Try to lock the forks
        if (pthread_mutex_trylock(&thread_info->forks[thread_info->left_fork_index]))
            if (pthread_mutex_trylock(&thread_info->forks[thread_info->right_fork_index]))
            {
                // Eat
                printf("Philosopher %i is eating, %i meals left\n", thread_info->id, i);

                // Unlock both forks
                pthread_mutex_unlock(&thread_info->forks[thread_info->left_fork_index]);
                pthread_mutex_unlock(&thread_info->forks[thread_info->right_fork_index]);
                i--;
            }   
            // Unlock the first fork to prevent deadlock
            else
                pthread_mutex_unlock(&thread_info->forks[thread_info->left_fork_index]);
    }

    // Print when finished
    printf("Philosopher %i is done\n", thread_info->id);
    
    // Cleanup
    free(thread_info);
    pthread_exit(NULL);
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

    // Initialize array of philosophers
    pthread_t *philosophers = malloc(sizeof(pthread_t) * num_philosophers);

    // Initialize array of forks
    pthread_mutex_t *forks = malloc(sizeof(pthread_mutex_t) * num_philosophers);
    for (int i = 0; i < num_philosophers; i++)
        pthread_mutex_init(&forks[i], NULL);

    
    // Start time
    start_time = CLOCK();


    // Start threads
    int error;
    for (int i = 0; i < num_philosophers; i++)
    {
        // Create thread info struct
        threadInfo *thread_info = malloc(sizeof(threadInfo));

        // Set variables for thread
        thread_info->id = i;
        thread_info->forks = forks;

        // Calculate fork indices
        thread_info->left_fork_index = i;
        thread_info->right_fork_index = (i + 1) % num_philosophers;

        // Create thread
        error = pthread_create(&philosophers[i], NULL, philosopher, thread_info);

        // Quit if error
        if (error)
        {
            printf("PTHREAD ERROR: pthread_create() code: %d\n", error);
            exit(1);
        }
    }
    

    // Wait for threads
    for (int i=0; i < num_philosophers; i++)
    {
        pthread_join(philosophers[i], NULL);
    }

    // End time
    end_time = CLOCK();

    // Print time
    total_time  = end_time - start_time;
    printf("Time: %f milliseconds\n", total_time);


    // Cleanup
    free(philosophers);
    free(forks);

    return 0;
}
