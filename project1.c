#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NITEMS 10        // number of items in shared buffer

// shared variables
char shared_buffer[NITEMS];    // echo buffer
int shared_count;        // item count

pthread_cond_t openFree;        // shared buffer full
pthread_cond_t usedFree;        // shared buffer empty
pthread_cond_t filled1;        // shared buffer empty
pthread_cond_t filled2;        // shared buffer empty

pthread_mutex_t mutex1;        // pthread mutex
pthread_mutex_t mutex2;        // pthread mutex
pthread_mutex_t mutex3;        // pthread mutex

unsigned int prod_index = 0;     // producer index into shared buffer
unsigned int cons_index = 0;     // consumer index into shard buffer

// function prototypes
void * thread1(void *arg);
void * thread2(void *arg);
void * thread3(void *arg);

int main() 
{ 
    pthread_t t1_tid, t2_tid, t3_tid; 

    // initialize pthread and conditional variables
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_cond_init(&openFree, NULL); // full shared buffer cond variable initialization
    pthread_cond_init(&usedFree, NULL); // empty shared buffer cond variable initialization
    pthread_cond_init(&filled1, NULL); // full shared buffer cond variable initialization
    pthread_cond_init(&filled2, NULL); // empty shared buffer cond variable initialization
    
    // start threads
    pthread_create(&t1_tid, NULL, thread1, NULL);
    pthread_create(&t2_tid1, NULL, thread2, NULL);
    pthread_create(&t3_tid2, NULL, thread3, NULL);
    
    // wait for threads to finish
    pthread_join(t1_tid, NULL);
    pthread_join(t2_tid, NULL);
    pthread_join(t3_tid, NULL);
            
    // clean up
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_cond_destroy(&openFree);
    pthread_cond_destroy(&usedFree);
    pthread_cond_destroy(&filled1);
    pthread_cond_destroy(&filled2);
    
    return 0;
}
// producer thread executes this function
void * thread1(void *arg)
{
    char key;

    printf("Enter text for producer to read and consumer to print, use Ctrl-C to exit.\n\n");

    // this loop has the producer read in from stdin and place on the shared buffer
    while (1)
    {
        // read input key
        scanf("%c", &key);

        // acquire mutex lock
        pthread_mutex_lock(&mutex);

        // -- if statement to check if shared buffer is full
        // -- if full, wait until signal from consumer thread
        // -- if not full signal the consumer thread to wakeup then finish process

        if (shared_count == NITEMS)
        {
            // sets thread to wait in while loop to prevent spurious wakeup
            while(shared_count == NITEMS)
                pthread_cond_wait(&sbfull, &mutex);
            // release mutex lock
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            // signal to wakeup consumer thread
            pthread_cond_signal(&sbnull);
                
            // store key in shared buffer
            shared_buffer[prod_index] = key;

            // update shared count variable
            shared_count++;
    
            // update producer index
            if (prod_index == NITEMS - 1)
                prod_index = 0;
            else
                prod_index++;
        
            // release mutex lock
            pthread_mutex_unlock(&mutex); 
        }
    }

    return NULL;
}
// consumer thread executes this function
void * thread2(void *arg)
{
    char key;

    long unsigned int id = (long unsigned int)pthread_self();

    // this loop has the consumer gets from the shared buffer and prints to stdout
    while (1)
    {


        // acquire mutex lock
        pthread_mutex_lock(&mutex);
        
        // -- if statement to check if shared buffer is empty
        // -- if empty, wait until signal from producer thread
        // -- if not empty signal the producer thread to wakeup then finish process


        if (shared_count == 0)
        {    
            // sets thread to wait in while loop to prevent spurious wakeup
            while(shared_count == 0)
                pthread_cond_wait(&sbnull, &mutex);
            // release mutex lock
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            // signal to wakeup producer thread
            pthread_cond_signal(&sbfull);

            // read key from shared buffer
            key = shared_buffer[cons_index];
        
            // echo key
            printf("consumer %lu: %c\n", (long unsigned int) id, key);

            // update shared count variable
            shared_count--;

            // update consumer index
            if (cons_index == NITEMS - 1)
                cons_index = 0;
            else
                cons_index++;
    
            // release mutex lock
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

void * thread3 (void *arg)
{

}